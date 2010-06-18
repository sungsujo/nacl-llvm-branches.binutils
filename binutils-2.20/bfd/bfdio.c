/* Low-level I/O routines for BFDs.

   Copyright 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998,
   1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.

   Written by Cygnus Support.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include <limits.h>
#include "bfd.h"
#include "libbfd.h"

#ifndef S_IXUSR
#define S_IXUSR 0100    /* Execute by owner.  */
#endif
#ifndef S_IXGRP
#define S_IXGRP 0010    /* Execute by group.  */
#endif
#ifndef S_IXOTH
#define S_IXOTH 0001    /* Execute by others.  */
#endif

#ifndef FD_CLOEXEC
#define FD_CLOEXEC 1
#endif

file_ptr
real_ftell (FILE *file)
{
#if defined (HAVE_FTELLO64)
  return ftello64 (file);
#elif defined (HAVE_FTELLO)
  return ftello (file);
#else
  return ftell (file);
#endif
}

int
real_fseek (FILE *file, file_ptr offset, int whence)
{
#if defined (HAVE_FSEEKO64)
  return fseeko64 (file, offset, whence);
#elif defined (HAVE_FSEEKO)
  return fseeko (file, offset, whence);
#else
  return fseek (file, offset, whence);
#endif
}

/* Mark FILE as close-on-exec.  Return FILE.  FILE may be NULL, in
   which case nothing is done.  */
static FILE *
close_on_exec (FILE *file)
{
#ifndef PNACL_TOOLCHAIN_SANDBOX
#if defined (HAVE_FILENO) && defined (F_GETFD)
  if (file)
    {
      int fd = fileno (file);
      int old = fcntl (fd, F_GETFD, 0);
      if (old >= 0)
	fcntl (fd, F_SETFD, old | FD_CLOEXEC);
    }
#endif
#endif
  return file;
}

FILE *
real_fopen (const char *filename, const char *modes)
{
#ifdef VMS
  char vms_modes[4];
  char *vms_attr;

  /* On VMS, fopen allows file attributes as optionnal arguments.
     We need to use them but we'd better to use the common prototype.
     In fopen-vms.h, they are separated from the mode with a comma.
     Split here.  */
  vms_attr = strchr (modes, ',');
  if (vms_attr == NULL)
    {
      /* No attributes.  */
      return close_on_exec (fopen (filename, modes));
    }
  else
    {
      /* Attributes found.  Split.  */
      size_t modes_len = strlen (modes) + 1;
      char attrs[modes_len + 1];
      char *at[3];
      int i;

      memcpy (attrs, modes, modes_len);
      at[0] = attrs;
      for (i = 0; i < 2; i++)
	{
	  at[i + 1] = strchr (at[i], ',');
	  BFD_ASSERT (at[i + 1] != NULL);
	  *(at[i + 1]++) = 0; /* Replace ',' with a nul, and skip it.  */
	}
      return close_on_exec (fopen (filename, at[0], at[1], at[2]));
    }
#else /* !VMS */
#if defined (HAVE_FOPEN64)
  return close_on_exec (fopen64 (filename, modes));
#else
  return close_on_exec (fopen (filename, modes));
#endif
#endif /* !VMS */
}

/*
INTERNAL_DEFINITION
	struct bfd_iovec

DESCRIPTION

	The <<struct bfd_iovec>> contains the internal file I/O class.
	Each <<BFD>> has an instance of this class and all file I/O is
	routed through it (it is assumed that the instance implements
	all methods listed below).

.struct bfd_iovec
.{
.  {* To avoid problems with macros, a "b" rather than "f"
.     prefix is prepended to each method name.  *}
.  {* Attempt to read/write NBYTES on ABFD's IOSTREAM storing/fetching
.     bytes starting at PTR.  Return the number of bytes actually
.     transfered (a read past end-of-file returns less than NBYTES),
.     or -1 (setting <<bfd_error>>) if an error occurs.  *}
.  file_ptr (*bread) (struct bfd *abfd, void *ptr, file_ptr nbytes);
.  file_ptr (*bwrite) (struct bfd *abfd, const void *ptr,
.                      file_ptr nbytes);
.  {* Return the current IOSTREAM file offset, or -1 (setting <<bfd_error>>
.     if an error occurs.  *}
.  file_ptr (*btell) (struct bfd *abfd);
.  {* For the following, on successful completion a value of 0 is returned.
.     Otherwise, a value of -1 is returned (and  <<bfd_error>> is set).  *}
.  int (*bseek) (struct bfd *abfd, file_ptr offset, int whence);
.  int (*bclose) (struct bfd *abfd);
.  int (*bflush) (struct bfd *abfd);
.  int (*bstat) (struct bfd *abfd, struct stat *sb);
.  {* Just like mmap: (void*)-1 on failure, mmapped address on success.  *}
.  void *(*bmmap) (struct bfd *abfd, void *addr, bfd_size_type len,
.                  int prot, int flags, file_ptr offset);
.};

*/


/* Return value is amount read.  */

bfd_size_type
bfd_bread (void *ptr, bfd_size_type size, bfd *abfd)
{
  size_t nread;

  /* If this is an archive element, don't read past the end of
     this element.  */
  if (abfd->arelt_data != NULL)
    {
      size_t maxbytes = ((struct areltdata *) abfd->arelt_data)->parsed_size;
      if (size > maxbytes)
	size = maxbytes;
    }

  if ((abfd->flags & BFD_IN_MEMORY) != 0)
    {
      struct bfd_in_memory *bim;
      bfd_size_type get;

      bim = (struct bfd_in_memory *) abfd->iostream;
      get = size;
      if (abfd->where + get > bim->size)
	{
	  if (bim->size < (bfd_size_type) abfd->where)
	    get = 0;
	  else
	    get = bim->size - abfd->where;
	  bfd_set_error (bfd_error_file_truncated);
	}
      memcpy (ptr, bim->buffer + abfd->where, (size_t) get);
      abfd->where += get;
      return get;
    }

  if (abfd->iovec)
    nread = abfd->iovec->bread (abfd, ptr, size);
  else
    nread = 0;
  if (nread != (size_t) -1)
    abfd->where += nread;

  return nread;
}

bfd_size_type
bfd_bwrite (const void *ptr, bfd_size_type size, bfd *abfd)
{
  size_t nwrote;

  if ((abfd->flags & BFD_IN_MEMORY) != 0)
    {
      struct bfd_in_memory *bim = (struct bfd_in_memory *) abfd->iostream;

      size = (size_t) size;
      if (abfd->where + size > bim->size)
	{
	  bfd_size_type newsize, oldsize;

	  oldsize = (bim->size + 127) & ~(bfd_size_type) 127;
	  bim->size = abfd->where + size;
	  /* Round up to cut down on memory fragmentation */
	  newsize = (bim->size + 127) & ~(bfd_size_type) 127;
	  if (newsize > oldsize)
	    {
	      bim->buffer = (bfd_byte *) bfd_realloc_or_free (bim->buffer,
                                                              newsize);
	      if (bim->buffer == NULL)
		{
		  bim->size = 0;
		  return 0;
		}
	      if (newsize > bim->size)
		memset (bim->buffer + bim->size, 0, newsize - bim->size);
	    }
	}
      memcpy (bim->buffer + abfd->where, ptr, (size_t) size);
      abfd->where += size;
      return size;
    }

  if (abfd->iovec)
    nwrote = abfd->iovec->bwrite (abfd, ptr, size);
  else
    nwrote = 0;

  if (nwrote != (size_t) -1)
    abfd->where += nwrote;
  if (nwrote != size)
    {
#ifdef ENOSPC
      errno = ENOSPC;
#endif
      bfd_set_error (bfd_error_system_call);
    }
  return nwrote;
}

file_ptr
bfd_tell (bfd *abfd)
{
  file_ptr ptr;

  if ((abfd->flags & BFD_IN_MEMORY) != 0)
    return abfd->where;

  if (abfd->iovec)
    {
      ptr = abfd->iovec->btell (abfd);

      if (abfd->my_archive)
	ptr -= abfd->origin;
    }
  else
    ptr = 0;

  abfd->where = ptr;
  return ptr;
}

int
bfd_flush (bfd *abfd)
{
  if ((abfd->flags & BFD_IN_MEMORY) != 0)
    return 0;

  if (abfd->iovec)
    return abfd->iovec->bflush (abfd);
  return 0;
}

/* Returns 0 for success, negative value for failure (in which case
   bfd_get_error can retrieve the error code).  */
int
bfd_stat (bfd *abfd, struct stat *statbuf)
{
  int result;

  if ((abfd->flags & BFD_IN_MEMORY) != 0)
    abort ();

  if (abfd->iovec)
    result = abfd->iovec->bstat (abfd, statbuf);
  else
    result = -1;

  if (result < 0)
    bfd_set_error (bfd_error_system_call);
  return result;
}

/* Returns 0 for success, nonzero for failure (in which case bfd_get_error
   can retrieve the error code).  */

int
bfd_seek (bfd *abfd, file_ptr position, int direction)
{
  int result;
  file_ptr file_position;
  /* For the time being, a BFD may not seek to it's end.  The problem
     is that we don't easily have a way to recognize the end of an
     element in an archive.  */

  BFD_ASSERT (direction == SEEK_SET || direction == SEEK_CUR);

  if (direction == SEEK_CUR && position == 0)
    return 0;

  if ((abfd->flags & BFD_IN_MEMORY) != 0)
    {
      struct bfd_in_memory *bim;

      bim = (struct bfd_in_memory *) abfd->iostream;

      if (direction == SEEK_SET)
	abfd->where = position;
      else
	abfd->where += position;

      if (abfd->where > bim->size)
	{
	  if (abfd->direction == write_direction
	      || abfd->direction == both_direction)
	    {
	      bfd_size_type newsize, oldsize;

	      oldsize = (bim->size + 127) & ~(bfd_size_type) 127;
	      bim->size = abfd->where;
	      /* Round up to cut down on memory fragmentation */
	      newsize = (bim->size + 127) & ~(bfd_size_type) 127;
	      if (newsize > oldsize)
	        {
		  bim->buffer = (bfd_byte *) bfd_realloc_or_free (bim->buffer,
                                                                  newsize);
		  if (bim->buffer == NULL)
		    {
		      bim->size = 0;
		      return -1;
		    }
		  memset (bim->buffer + oldsize, 0, newsize - oldsize);
	        }
	    }
	  else
	    {
	      abfd->where = bim->size;
	      bfd_set_error (bfd_error_file_truncated);
	      return -1;
	    }
	}
      return 0;
    }

  if (abfd->format != bfd_archive && abfd->my_archive == 0)
    {
      if (direction == SEEK_SET && (bfd_vma) position == abfd->where)
	return 0;
    }
  else
    {
      /* We need something smarter to optimize access to archives.
	 Currently, anything inside an archive is read via the file
	 handle for the archive.  Which means that a bfd_seek on one
	 component affects the `current position' in the archive, as
	 well as in any other component.

	 It might be sufficient to put a spike through the cache
	 abstraction, and look to the archive for the file position,
	 but I think we should try for something cleaner.

	 In the meantime, no optimization for archives.  */
    }

  file_position = position;
  if (direction == SEEK_SET && abfd->my_archive != NULL)
    file_position += abfd->origin;

  if (abfd->iovec)
    result = abfd->iovec->bseek (abfd, file_position, direction);
  else
    result = -1;

  if (result != 0)
    {
      int hold_errno = errno;

      /* Force redetermination of `where' field.  */
      bfd_tell (abfd);

      /* An EINVAL error probably means that the file offset was
         absurd.  */
      if (hold_errno == EINVAL)
	bfd_set_error (bfd_error_file_truncated);
      else
	{
	  bfd_set_error (bfd_error_system_call);
	  errno = hold_errno;
	}
    }
  else
    {
      /* Adjust `where' field.  */
      if (direction == SEEK_SET)
	abfd->where = position;
      else
	abfd->where += position;
    }
  return result;
}

/*
FUNCTION
	bfd_get_mtime

SYNOPSIS
	long bfd_get_mtime (bfd *abfd);

DESCRIPTION
	Return the file modification time (as read from the file system, or
	from the archive header for archive members).

*/

long
bfd_get_mtime (bfd *abfd)
{
  struct stat buf;

  if (abfd->mtime_set)
    return abfd->mtime;

  if (abfd->iovec == NULL)
    return 0;

  if (abfd->iovec->bstat (abfd, &buf) != 0)
    return 0;

  abfd->mtime = buf.st_mtime;		/* Save value in case anyone wants it */
  return buf.st_mtime;
}

/*
FUNCTION
	bfd_get_size

SYNOPSIS
	file_ptr bfd_get_size (bfd *abfd);

DESCRIPTION
	Return the file size (as read from file system) for the file
	associated with BFD @var{abfd}.

	The initial motivation for, and use of, this routine is not
	so we can get the exact size of the object the BFD applies to, since
	that might not be generally possible (archive members for example).
	It would be ideal if someone could eventually modify
	it so that such results were guaranteed.

	Instead, we want to ask questions like "is this NNN byte sized
	object I'm about to try read from file offset YYY reasonable?"
	As as example of where we might do this, some object formats
	use string tables for which the first <<sizeof (long)>> bytes of the
	table contain the size of the table itself, including the size bytes.
	If an application tries to read what it thinks is one of these
	string tables, without some way to validate the size, and for
	some reason the size is wrong (byte swapping error, wrong location
	for the string table, etc.), the only clue is likely to be a read
	error when it tries to read the table, or a "virtual memory
	exhausted" error when it tries to allocate 15 bazillon bytes
	of space for the 15 bazillon byte table it is about to read.
	This function at least allows us to answer the question, "is the
	size reasonable?".
*/

file_ptr
bfd_get_size (bfd *abfd)
{
  struct stat buf;

  if ((abfd->flags & BFD_IN_MEMORY) != 0)
    return ((struct bfd_in_memory *) abfd->iostream)->size;

  if (abfd->iovec == NULL)
    return 0;

  if (abfd->iovec->bstat (abfd, &buf) != 0)
    return 0;

  return buf.st_size;
}


/*
FUNCTION
	bfd_mmap

SYNOPSIS
	void *bfd_mmap (bfd *abfd, void *addr, bfd_size_type len,
	                int prot, int flags, file_ptr offset);

DESCRIPTION
	Return mmap()ed region of the file, if possible and implemented.

*/

void *
bfd_mmap (bfd *abfd, void *addr, bfd_size_type len,
	  int prot, int flags, file_ptr offset)
{
  void *ret = (void *)-1;
  if ((abfd->flags & BFD_IN_MEMORY) != 0)
    return ret;

  if (abfd->iovec == NULL)
    return ret;

  return abfd->iovec->bmmap (abfd, addr, len, prot, flags, offset);
}
