# Vulkan4Aros source lines checked during the engine-design reviews

> Source: read-only reads of ~/Work/projects/Vulkan4Aros (working tree as of 2026-10-30)
> Collected: 2026-10-30
> Published: Unknown

Verbatim lines, each prefixed with `path:line:`. Collected while reviewing engine design revisions 3-5.

## Loader: instance extensions offered only when advertised

```
src/vulkan/loader/vk_aros_loader.c:406: static VkResult VKAPI_CALL boundary_r7_create_instance(const VkInstanceCreateInfo *pCreateInfo,
src/vulkan/loader/vk_aros_loader.c:409: {
src/vulkan/loader/vk_aros_loader.c:410:     static const char *const kAdd[2] = {
src/vulkan/loader/vk_aros_loader.c:414:     const char **names;
src/vulkan/loader/vk_aros_loader.c:415:     /* Offer each surface extension ONLY when the ICD actually advertises it.
src/vulkan/loader/vk_aros_loader.c:416:      * Appending unconditionally made every instance creation on a driver
src/vulkan/loader/vk_aros_loader.c:417:      * without WSI fail: C:VKSUBMIT against NVK came back vkCreateInstance
src/vulkan/loader/vk_aros_loader.c:418:      * rc=-7 (VK_ERROR_EXTENSION_NOT_PRESENT) with the app having requested
src/vulkan/loader/vk_aros_loader.c:419:      * ZERO extensions. Measured on the card by C:Extras/VKINSTEXT, NVK
src/vulkan/loader/vk_aros_loader.c:420:      * advertises 10 instance extensions and NEITHER VK_KHR_surface NOR
src/vulkan/loader/vk_aros_loader.c:421:      * VK_KHR_xcb_surface is among them -- it gates its WSI behind
src/vulkan/loader/vk_aros_loader.c:422:      * NVK_USE_WSI_PLATFORM -- so asking for them is asking for something the
src/vulkan/loader/vk_aros_loader.c:423:      * ICD does not have. Khronos requires an implementation to reject an
src/vulkan/loader/vk_aros_loader.c:424:      * unsupported extension, so the boundary was at fault, not the driver.
src/vulkan/loader/vk_aros_loader.c:425:      * This is the same ask-first shape boundary_r7_create_device already uses
src/vulkan/loader/vk_aros_loader.c:426:      * for VK_KHR_swapchain (commit c0b005ce); this path just never got it.
```

## AHI device: units and CMD_WRITE

```
src/abi/v11/AROS/workbench/devs/AHI/Include/C/devices/ahi.h:354: #define AHI_DEFAULT_UNIT	(0U)
src/abi/v11/AROS/workbench/devs/AHI/Include/C/devices/ahi.h:355: #define AHI_NO_UNIT		(255U)
src/abi/v11/AROS/workbench/devs/AHI/Device/device.c:202: *       unit - Either AHI_DEFAULT_UNIT (0), AHI_NO_UNIT (255) or any other
src/abi/v11/AROS/workbench/devs/AHI/Device/device.c:203: *           unit the user has requested, for example with a UNIT tooltype.
src/abi/v11/AROS/workbench/devs/AHI/Device/device.c:204: *           AHI_NO_UNIT should be used when you're using the low-level
src/abi/v11/AROS/workbench/devs/AHI/Device/device.c:205: *           API.
src/abi/v11/AROS/workbench/devs/AHI/Device/device.c:325:     else if(unit == AHI_NO_UNIT)
src/abi/v11/AROS/workbench/devs/AHI/Device/device.c:326:       InitUnit(unit,AHIBase);
src/abi/v11/AROS/workbench/devs/AHI/Device/device.c:331:     ioreq->ahir_Std.io_Unit=(struct Unit *) iounit;
src/abi/v11/AROS/workbench/devs/AHI/Device/device.c:332:     if(iounit)    // Is NULL for AHI_NO_UNIT
src/abi/v11/AROS/workbench/devs/AHI/Device/devcommands.c:106:     case CMD_WRITE:
src/abi/v11/AROS/workbench/devs/AHI/Device/devcommands.c:107:     case CMD_START:
src/abi/v11/AROS/workbench/devs/AHI/Device/devcommands.c:108:       ioreq->ahir_Std.io_Flags &= ~IOF_QUICK;
src/abi/v11/AROS/workbench/devs/AHI/Device/devcommands.c:109:       PutMsg(&ioreq->ahir_Std.io_Unit->unit_MsgPort,&ioreq->ahir_Std.io_Message);
```

## AmigaDOS Rename: where ERROR_OBJECT_EXISTS is raised

```
src/abi/v11/AROS/rom/filesys/fat/ops.c:574:             return ERROR_OBJECT_EXISTS;
src/abi/v11/AROS/rom/filesys/afs/filehandles2.c:503:                 return ERROR_OBJECT_EXISTS;
grep -c ERROR_OBJECT_EXISTS src/abi/v11/AROS/rom/dos/rename.c: 0
```

## PT-AHIPlay.s (Protracker V2.3A playroutine adapted for ahi.device)

```
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:13: *    ----- Protracker V2.3A Playroutine -----    *
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:16: * Adapted for 'ahi.device' by Martin Blom.
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:25: *	AHIA_MinPlayerFreq	Should be (32*2/5)<<16
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:26: *	AHIA_MaxPlayerFreq	Should be (255*2/5)<<16
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:353: SetTempo
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:354: 	CMP.W	#32,D0
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:355: 	BHS.S	setemsk
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:356: 	MOVEQ	#32,D0
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:412: 	move.w	pe_Period(a4),d2
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:414: 	move.l	#3546895,d1
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:415: 	divu.w	d2,d1
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:1138: 	CMP.B	#$40,n_volume(A6)
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:1140: 	MOVE.B	#$40,n_volume(A6)
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:1198: mt_SetSpeed
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:1200: 	MOVE.B	3(A6),D0
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:1201: 	BEQ	mt_end
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:1202: 	CMP.B	#32,D0
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:1203: 	BHS	SetTempo
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:1205: 	MOVE.B	D0,ptd_speed(a2)
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:1222: 	CMP.B	#$C,D0
src/abi/v11/AROS/workbench/devs/AHI/Examples/Low-level/PT-AHIPlay/PT-AHIPlay.s:1223: 	BEQ	mt_VolumeChange
```

## Vulkan4Aros git history checked (rev-4 report attribution)

```
bf63c028 aros-v1-merge-fixes Stage 2: corrected chain F3+F1 (records)
```
