	.text
	.def	 @feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.intel_syntax noprefix
	.file	"vector.cpp"
	.def	 "?Fuck@@YANN@Z";
	.scl	2;
	.type	32;
	.endef
	.globl	"?Fuck@@YANN@Z"         # -- Begin function ?Fuck@@YANN@Z
	.p2align	4, 0x90
"?Fuck@@YANN@Z":                        # @"?Fuck@@YANN@Z"
# %bb.0:
	jmp	tan                     # TAILCALL
                                        # -- End function
	.section	.drectve,"yn"
	.ascii	" /FAILIFMISMATCH:\"_MSC_VER=1900\""
	.ascii	" /FAILIFMISMATCH:\"_ITERATOR_DEBUG_LEVEL=0\""
	.ascii	" /FAILIFMISMATCH:\"RuntimeLibrary=MT_StaticRelease\""
	.ascii	" /DEFAULTLIB:libcpmt.lib"
	.ascii	" /FAILIFMISMATCH:\"_CRT_STDIO_ISO_WIDE_SPECIFIERS=0\""

	.addrsig
