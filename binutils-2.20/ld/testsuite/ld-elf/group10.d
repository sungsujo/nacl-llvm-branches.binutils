#source: group10.s
#ld: -r -T group.ld
#readelf: -Sg --wide

#...
group section \[[ 0-9]+\] `foo_group' \[foo_group\] contains 4 sections:
   \[Index\]    Name
   \[[ 0-9]+\]   \.text.*
   \[[ 0-9]+\]   \.rodata\.str.*
   \[[ 0-9]+\]   \.data.*
   \[[ 0-9]+\]   \.keepme.*
