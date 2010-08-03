# Check FMA4 instructions

	.allow_index_reg
	.text
_start:

	vfmaddpd %ymm4,%ymm6,%ymm2,%ymm7
	vfmaddpd (%ecx),%ymm6,%ymm2,%ymm7
	vfmaddps %ymm4,%ymm6,%ymm2,%ymm7
	vfmaddps (%ecx),%ymm6,%ymm2,%ymm7
	vfmaddsubpd %ymm4,%ymm6,%ymm2,%ymm7
	vfmaddsubpd (%ecx),%ymm6,%ymm2,%ymm7
	vfmaddsubps %ymm4,%ymm6,%ymm2,%ymm7
	vfmaddsubps (%ecx),%ymm6,%ymm2,%ymm7
	vfmsubaddpd %ymm4,%ymm6,%ymm2,%ymm7
	vfmsubaddpd (%ecx),%ymm6,%ymm2,%ymm7
	vfmsubaddps %ymm4,%ymm6,%ymm2,%ymm7
	vfmsubaddps (%ecx),%ymm6,%ymm2,%ymm7
	vfmsubpd %ymm4,%ymm6,%ymm2,%ymm7
	vfmsubpd (%ecx),%ymm6,%ymm2,%ymm7
	vfmsubps %ymm4,%ymm6,%ymm2,%ymm7
	vfmsubps (%ecx),%ymm6,%ymm2,%ymm7
	vfmaddpd %xmm4,%xmm6,%xmm2,%xmm7
	vfmaddpd (%ecx),%xmm6,%xmm2,%xmm7
	vfmaddpd %xmm4,(%ecx),%xmm2,%xmm7
	vfmaddps %xmm4,%xmm6,%xmm2,%xmm7
	vfmaddps (%ecx),%xmm6,%xmm2,%xmm7
	vfmaddps %xmm4,(%ecx),%xmm2,%xmm7
	vfmaddsubpd %xmm4,%xmm6,%xmm2,%xmm7
	vfmaddsubpd (%ecx),%xmm6,%xmm2,%xmm7
	vfmaddsubpd %xmm4,(%ecx),%xmm2,%xmm7
	vfmaddsubps %xmm4,%xmm6,%xmm2,%xmm7
	vfmaddsubps (%ecx),%xmm6,%xmm2,%xmm7
	vfmaddsubps %xmm4,(%ecx),%xmm2,%xmm7
	vfmsubaddpd %xmm4,%xmm6,%xmm2,%xmm7
	vfmsubaddpd (%ecx),%xmm6,%xmm2,%xmm7
	vfmsubaddpd %xmm4,(%ecx),%xmm2,%xmm7
	vfmsubaddps %xmm4,%xmm6,%xmm2,%xmm7
	vfmsubaddps (%ecx),%xmm6,%xmm2,%xmm7
	vfmsubaddps %xmm4,(%ecx),%xmm2,%xmm7
	vfmsubpd %xmm4,%xmm6,%xmm2,%xmm7
	vfmsubpd (%ecx),%xmm6,%xmm2,%xmm7
	vfmsubpd %xmm4,(%ecx),%xmm2,%xmm7
	vfmsubps %xmm4,%xmm6,%xmm2,%xmm7
	vfmsubps (%ecx),%xmm6,%xmm2,%xmm7
	vfmsubps %xmm4,(%ecx),%xmm2,%xmm7
	vfmaddsd %xmm4,%xmm6,%xmm2,%xmm7
	vfmaddsd (%ecx),%xmm6,%xmm2,%xmm7
	vfmaddsd %xmm4,(%ecx),%xmm2,%xmm7
	vfmsubsd %xmm4,%xmm6,%xmm2,%xmm7
	vfmsubsd (%ecx),%xmm6,%xmm2,%xmm7
	vfmsubsd %xmm4,(%ecx),%xmm2,%xmm7
	vfmaddss %xmm4,%xmm6,%xmm2,%xmm7
	vfmaddss (%ecx),%xmm6,%xmm2,%xmm7
	vfmaddss %xmm4,(%ecx),%xmm2,%xmm7
	vfmsubss %xmm4,%xmm6,%xmm2,%xmm7
	vfmsubss (%ecx),%xmm6,%xmm2,%xmm7
	vfmsubss %xmm4,(%ecx),%xmm2,%xmm7
	vfnmaddpd %ymm4,%ymm6,%ymm2,%ymm7
	vfnmaddpd (%ecx),%ymm6,%ymm2,%ymm7
	vfnmaddps %ymm4,%ymm6,%ymm2,%ymm7
	vfnmaddps (%ecx),%ymm6,%ymm2,%ymm7
	vfnmsubpd %ymm4,%ymm6,%ymm2,%ymm7
	vfnmsubpd (%ecx),%ymm6,%ymm2,%ymm7
	vfnmsubps %ymm4,%ymm6,%ymm2,%ymm7
	vfnmsubps (%ecx),%ymm6,%ymm2,%ymm7
	vfnmaddpd %xmm4,%xmm6,%xmm2,%xmm7
	vfnmaddpd (%ecx),%xmm6,%xmm2,%xmm7
	vfnmaddpd %xmm4,(%ecx),%xmm2,%xmm7
	vfnmaddps %xmm4,%xmm6,%xmm2,%xmm7
	vfnmaddps (%ecx),%xmm6,%xmm2,%xmm7
	vfnmaddps %xmm4,(%ecx),%xmm2,%xmm7
	vfnmsubpd %xmm4,%xmm6,%xmm2,%xmm7
	vfnmsubpd (%ecx),%xmm6,%xmm2,%xmm7
	vfnmsubpd %xmm4,(%ecx),%xmm2,%xmm7
	vfnmsubps %xmm4,%xmm6,%xmm2,%xmm7
	vfnmsubps (%ecx),%xmm6,%xmm2,%xmm7
	vfnmsubps %xmm4,(%ecx),%xmm2,%xmm7
	vfnmaddsd %xmm4,%xmm6,%xmm2,%xmm7
	vfnmaddsd (%ecx),%xmm6,%xmm2,%xmm7
	vfnmaddsd %xmm4,(%ecx),%xmm2,%xmm7
	vfnmsubsd %xmm4,%xmm6,%xmm2,%xmm7
	vfnmsubsd (%ecx),%xmm6,%xmm2,%xmm7
	vfnmsubsd %xmm4,(%ecx),%xmm2,%xmm7
	vfnmaddss %xmm4,%xmm6,%xmm2,%xmm7
	vfnmaddss (%ecx),%xmm6,%xmm2,%xmm7
	vfnmaddss %xmm4,(%ecx),%xmm2,%xmm7
	vfnmsubss %xmm4,%xmm6,%xmm2,%xmm7
	vfnmsubss (%ecx),%xmm6,%xmm2,%xmm7
	vfnmsubss %xmm4,(%ecx),%xmm2,%xmm7

