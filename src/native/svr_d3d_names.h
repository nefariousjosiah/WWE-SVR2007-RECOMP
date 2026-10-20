// SvR 2007 addresses for the XDK D3D functions that re:Blue's renderer hooks by name.
//
// Force-included into the native renderer sources (cmake/native_renderer.cmake), so
// REX_HOOK(D3DDevice_Clear, ...) overrides SvR's recompiled sub_821D93F0 and
// __imp__D3DDevice_Clear calls its original body. Every entry is confirmed by behaviour (the device
// fields it writes and the functions it calls), matched from SvR 2008's map with
// tools/native/match_functions.py. SvR 2007's XDK keeps the whole library, draws included, in
// 0x821D2000..0x821EC000. Functions not listed here are either not used by SvR's engine or not
// identified yet; a hook that names one fails to compile on purpose.

#pragma once

// Device / frame. Present (sub_821EB100) calls, as in 2008: SetRenderTarget, Synchronize, Resolve,
// SetViewport sub_821D7768, SetScissorRect, Swap.
#define D3DDevice_Present                     sub_821EB100
#define D3DDevice_Swap                        sub_821EAB50  // calls VdSwap (0x821EACDC)
#define D3DDevice_SynchronizeToPresentationInterval sub_821EA9C8
#define D3DDevice_Resolve                     sub_821DB9C0
#define D3DDevice_Clear                       sub_821D93F0
#define D3DDevice_SetRenderTarget             sub_821D7F60  // device + (index+3046)*4
#define D3DDevice_SetDepthStencilSurface      sub_821D8290  // device + 0x2FA8
#define D3DDevice_SetScissorRect              sub_821D76A0  // device + 0x3080

// Draws (same calls as 2008's; DrawIndexedVertices reads the index buffer at device + 0x2F94).
#define D3DDevice_BeginVertices               sub_821D94A0
#define D3DDevice_EndVertices                 sub_821D9950
#define D3DDevice_DrawVertices                sub_821D9970
#define D3DDevice_DrawIndexedVertices         sub_821D9D78

// Buffers: the game creates, locks and unlocks its own; the renderer only observes Unlock
// (svr_geometry.cpp). Both Unlocks are 2008's exactly (tail calls into sub_821D2C98).
// D3DDevice_CreateVertexBuffer sub_821D2EE8 and D3DDevice_CreateIndexBuffer sub_821D3020 are
// matched but deliberately NOT mapped: gpu/hooks/resource.cpp's REX_HOOKs would replace them.
#define D3DVertexBuffer_Unlock                sub_821D3010
#define D3DIndexBuffer_Unlock                 sub_821D3130

// Resource creation. CreateVertexDeclaration counts elements to the 0xFF stream and copies them
// to +52 of a count*12+56 object (2008's layout); CreateVertexShader clears a 592-byte object
// (2008: 872), CreatePixelShader a 56-byte one.
#define D3DDevice_CreateVertexDeclaration     sub_821DAEA0
#define D3DDevice_CreateVertexShader          sub_821DB120
#define D3DDevice_CreatePixelShader           sub_821DB030

// State
#define D3DDevice_SetTexture                  sub_821D5058  // fetch constants device + 1024 + s*24
#define D3DDevice_SetVertexShader             sub_821DACE0  // device + 0x3094
#define D3DDevice_SetPixelShader              sub_821DAAA8  // device + 0x3090
#define D3DDevice_SetVertexDeclaration        sub_821DAE80  // device + 0x2D20
#define D3DDevice_SetStreamSource             sub_821D79A8  // device + (stream+3051)*4
#define D3DDevice_SetIndices                  sub_821D7B58  // device + 0x2F94

// Original bodies (__imp__ = the recompiled function, for hooks that call through).
#define __imp__D3DDevice_Present              __imp__sub_821EB100
#define __imp__D3DDevice_Swap                 __imp__sub_821EAB50
#define __imp__D3DDevice_Resolve              __imp__sub_821DB9C0
#define __imp__D3DDevice_Clear                __imp__sub_821D93F0
#define __imp__D3DDevice_SetTexture           __imp__sub_821D5058
#define __imp__D3DDevice_SetRenderTarget      __imp__sub_821D7F60
#define __imp__D3DDevice_SetDepthStencilSurface __imp__sub_821D8290
#define __imp__D3DDevice_SetScissorRect       __imp__sub_821D76A0
#define __imp__D3DDevice_BeginVertices        __imp__sub_821D94A0
#define __imp__D3DDevice_EndVertices          __imp__sub_821D9950
#define __imp__D3DDevice_DrawVertices         __imp__sub_821D9970
#define __imp__D3DDevice_DrawIndexedVertices  __imp__sub_821D9D78
#define __imp__D3DDevice_CreateVertexDeclaration __imp__sub_821DAEA0
#define __imp__D3DDevice_CreateVertexShader   __imp__sub_821DB120
#define __imp__D3DDevice_CreatePixelShader    __imp__sub_821DB030
#define __imp__D3DDevice_SetVertexShader      __imp__sub_821DACE0
#define __imp__D3DDevice_SetPixelShader       __imp__sub_821DAAA8
#define __imp__D3DDevice_SetVertexDeclaration __imp__sub_821DAE80
#define __imp__D3DDevice_SetStreamSource      __imp__sub_821D79A8
#define __imp__D3DDevice_SetIndices           __imp__sub_821D7B58
#define __imp__D3DVertexBuffer_Unlock         __imp__sub_821D3010
#define __imp__D3DIndexBuffer_Unlock          __imp__sub_821D3130

// Resource description getters (re:Blue answers them for host-created resources and calls the
// original otherwise); exact matches of 2008's.
#define __imp__D3DSurface_GetDesc             __imp__sub_821D4F38
#define __imp__D3DTexture_GetLevelDesc        __imp__sub_821D3328
#define __imp__D3DVolumeTexture_GetLevelDesc  __imp__sub_821D4400
