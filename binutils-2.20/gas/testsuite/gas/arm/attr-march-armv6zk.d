# name: attributes for -march=armv6zk
# source: blank.s
# as: -march=armv6zk
# readelf: -A
# This test is only valid on EABI based ports.
# target: *-*-*eabi

Attribute Section: aeabi
File Attributes
  Tag_CPU_name: "6ZK"
  Tag_CPU_arch: v6K
  Tag_ARM_ISA_use: Yes
  Tag_THUMB_ISA_use: Thumb-1
  Tag_DIV_use: Not allowed
