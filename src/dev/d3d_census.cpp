// Developer call census of D3D library functions (generated code).
// Only built with -DSVR_D3D_CENSUS=ON. Do not edit by hand.

#include "generated/default/svr2007_init.h"
#include "d3d_census_runtime.h"

static constexpr uint32_t kCensusAddrs[] = {
    0x8222E0E8, 0x8222E278, 0x8222E3A8, 0x8222E498, 0x8222E588, 0x8222E630,
    0x8222E6A0, 0x8222E7A0, 0x8222E828, 0x8222E888, 0x8222E980, 0x8222EA18,
    0x8222EA88, 0x8222EBC0, 0x8222EC68, 0x8222F068, 0x8222F168, 0x8222F480,
    0x8222F508, 0x8222F578, 0x8222F580, 0x8222F590, 0x8222F598, 0x8222F630,
    0x8222F648, 0x8222F650, 0x8222F660, 0x8222F810, 0x8222F848, 0x8222F868,
    0x8222F8B0, 0x8222F8B8, 0x8222F958, 0x8222F998, 0x8222FA18, 0x8222FA58,
    0x8222FA70, 0x8222FB20, 0x8222FB28, 0x8222FBB8, 0x8222FBC8, 0x8222FBD8,
    0x8222FC10, 0x8222FC18, 0x8222FC28, 0x8222FC50, 0x8222FC90, 0x8222FD20,
    0x8222FDD8, 0x8222FE60, 0x82230208, 0x82230230, 0x822302F0, 0x82230614,
    0x82230AE0, 0x82230E70, 0x82231390, 0x82231398, 0x822313E0, 0x82231428,
    0x82231460, 0x82231478, 0x822314B0, 0x822314D0, 0x82231538, 0x82231590,
    0x82231598, 0x82231608, 0x82231780, 0x822317E0, 0x82231870, 0x822318D0,
    0x82231940, 0x822319D8, 0x822319E8, 0x82231AC8, 0x82231B40, 0x82231C18,
    0x82231C70, 0x82231E30, 0x82231F50, 0x822321E0, 0x822325E8, 0x82232720,
    0x82232C90, 0x82232D38, 0x82232EA0, 0x822330E0, 0x82233448, 0x822334E0,
    0x822339E0, 0x822339F8, 0x82234264, 0x8223428C, 0x822342E0, 0x8223454C,
    0x82234574, 0x822345C8, 0x82234C48, 0x82234DC0, 0x82234DE4, 0x82234E04,
    0x82234E48, 0x82234E70, 0x82234E80, 0x82234E90, 0x82234EB8, 0x82234F14,
    0x82234F38, 0x82234F40, 0x82234FD0, 0x82234FD8, 0x82235030, 0x822351C0,
    0x822353F8, 0x82235500, 0x82235578, 0x822355F8, 0x82235648, 0x82235710,
    0x82235760, 0x82235770, 0x82235820, 0x82235868, 0x82235878, 0x82235910,
    0x82235928, 0x82235940, 0x822359B0, 0x82235A48, 0x82235AD0, 0x82235BF0,
    0x82235F60, 0x822360B8, 0x82236480, 0x82236580, 0x82236680, 0x82236B40,
    0x82236C08, 0x82236C98, 0x82236D40, 0x82237050, 0x822372A8, 0x822372B8,
    0x82237370, 0x82237388, 0x82237440, 0x82237448, 0x822374F8, 0x82237500,
    0x82237620, 0x82237748, 0x822377E0, 0x82237800, 0x82237848, 0x82237868,
    0x822379E0, 0x82237A78, 0x82237B28, 0x82237C18, 0x82237C88, 0x82237CE0,
    0x82237D98, 0x82237E38, 0x82237E60, 0x82238068, 0x82238150, 0x82238338,
    0x822384F8, 0x822385E0, 0x822386B8, 0x82238780, 0x822388F0, 0x82238910,
    0x82238930, 0x82238A28, 0x82238B80, 0x82238D08, 0x82238E08, 0x82238E40,
    0x82238EB8, 0x82238F20, 0x82238F88, 0x82239000, 0x82239058, 0x82239468,
    0x82239518, 0x82239570, 0x82239578, 0x82239620, 0x822396C8, 0x82239748,
    0x822397A8, 0x822397F8, 0x822398D8, 0x82239A10, 0x82239A60, 0x82239BD8,
    0x82239CB8, 0x82239CE8, 0x82239DA8, 0x82239DC0, 0x82239E18, 0x82239E68,
    0x82239F40, 0x82239F98, 0x82239FD8, 0x82239FE8, 0x8223A0F8, 0x8223A1B0,
    0x8223A220, 0x8223A320, 0x8223A4B8, 0x8223A660, 0x8223A700, 0x8223A780,
    0x8223A958, 0x8223A960, 0x8223AEF8, 0x8223B028, 0x8223B088, 0x8223B098,
    0x8223B0B0, 0x8223B130, 0x8223B398, 0x8223B468, 0x8223B6E8, 0x8223B788,
    0x8223B840, 0x8223BA40, 0x8223BDD8, 0x8223BE98, 0x8223C018, 0x8223C178,
    0x8223C2D8, 0x8223C8E8, 0x8223C9B8, 0x8223CA70, 0x8223CBC0, 0x8223CD20,
    0x8223CDD0, 0x8223CF10, 0x8223D740, 0x8223DA20, 0x8223DA40, 0x8223DA50,
    0x8223DA70, 0x8223DA80, 0x8223DAA8, 0x8223DAB8, 0x8223DB38, 0x8223DB48,
    0x8223DBC8, 0x8223DBD8, 0x8223DC58, 0x8223DC68, 0x8223DCE8, 0x8223DCF8,
    0x8223DD58, 0x8223DD68, 0x8223DDC8, 0x8223DDD8, 0x8223DE38, 0x8223DE48,
    0x8223DED0, 0x8223DEE0, 0x8223DF18, 0x8223DF48, 0x8223DF68, 0x8223DF78,
    0x8223E018, 0x8223E098, 0x8223E0C0, 0x8223E0D0, 0x8223E110, 0x8223E140,
    0x8223E160, 0x8223E170, 0x8223E1A8, 0x8223E1B0, 0x8223E1D0, 0x8223E1E0,
    0x8223E208, 0x8223E218, 0x8223E250, 0x8223E258, 0x8223E280, 0x8223E290,
    0x8223E2B0, 0x8223E2C0, 0x8223E2E8, 0x8223E2F8, 0x8223E320, 0x8223E330,
    0x8223E350, 0x8223E360, 0x8223E380, 0x8223E390, 0x8223E3B8, 0x8223E3C8,
    0x8223E3F0, 0x8223E400, 0x8223E420, 0x8223E430, 0x8223E448, 0x8223E450,
    0x8223E468, 0x8223E470, 0x8223E488, 0x8223E490, 0x8223E4A8, 0x8223E4B0,
    0x8223E4C8, 0x8223E4D0, 0x8223E4E8, 0x8223E4F0, 0x8223E530, 0x8223E540,
    0x8223E548, 0x8223E5F0, 0x8223E610, 0x8223E6A8, 0x8223E6B8, 0x8223E6D8,
    0x8223E6E8, 0x8223E700, 0x8223E708, 0x8223E740, 0x8223E748, 0x8223E780,
    0x8223E788, 0x8223E7C0, 0x8223E7C8, 0x8223E800, 0x8223E808, 0x8223E810,
    0x8223E818, 0x8223E868, 0x8223E878, 0x8223E8C0, 0x8223E8D0, 0x8223E918,
    0x8223E928, 0x8223E948, 0x8223E970, 0x8223E998, 0x8223E9C0, 0x8223E9E8,
    0x8223EA10, 0x8223EA38, 0x8223EA58, 0x8223EA78, 0x8223EAA0, 0x8223EAC8,
    0x8223EAF0, 0x8223EB18, 0x8223EB40, 0x8223EB68, 0x8223EB88, 0x8223EB98,
    0x8223EBA8, 0x8223EBB8, 0x8223EBC8, 0x8223EBD8, 0x8223EBE8, 0x8223EBF8,
    0x8223EC08, 0x8223EC18, 0x8223EC28, 0x8223EC38, 0x8223EC48, 0x8223EC58,
    0x8223EC68, 0x8223EC78, 0x8223EC88, 0x8223ECC8, 0x8223ECD8, 0x8223ED80,
    0x8223ED88, 0x8223EE30, 0x8223EE38, 0x8223EEE0, 0x8223EEE8, 0x8223EF90,
    0x8223EF98, 0x8223EFC0, 0x8223EFD0, 0x8223EFF8, 0x8223F008, 0x8223F030,
    0x8223F040, 0x8223F068, 0x8223F078, 0x8223F098, 0x8223F0A8, 0x8223F0C8,
    0x8223F0D0, 0x8223F0F0, 0x8223F100, 0x8223F120, 0x8223F138, 0x8223F160,
    0x8223F168, 0x8223F190, 0x8223F198, 0x8223F1B8, 0x8223F1C0, 0x8223F1E8,
    0x8223F1F0, 0x8223F210, 0x8223F220, 0x8223F278, 0x8223F288, 0x8223F2A8,
    0x8223F2B8, 0x8223F2D8, 0x8223F2E8, 0x8223F308, 0x8223F318, 0x8223F330,
    0x8223F338, 0x8223F340, 0x8223F348, 0x8223F358, 0x8223F368, 0x8223F428,
    0x8223F460, 0x8223F4E8, 0x8223F4F8, 0x8223F5B8, 0x8223F5F0, 0x8223F678,
    0x8223F688, 0x8223F6C8, 0x8223F6E0, 0x8223F768, 0x8223F778, 0x8223F7E0,
    0x8223F7F0, 0x8223F850, 0x8223F898, 0x8223F8F8, 0x8223F938, 0x8223F9A8,
    0x8223F9B8, 0x8223FA28, 0x8223FA38, 0x8223FA88, 0x8223FAA8, 0x8223FAE0,
    0x8223FAF8, 0x8223FB30, 0x8223FB48, 0x8223FB80, 0x8223FB98, 0x8223FBD8,
    0x8223FBF0, 0x8223FC30, 0x8223FC48, 0x8223FC88, 0x8223FCA0, 0x8223FCE0,
    0x8223FCF8, 0x8223FD40, 0x8223FD58, 0x8223FE58, 0x8223FEA8, 0x8223FFC8,
    0x82240050, 0x822400E0, 0x82240128, 0x82240168, 0x822401A8, 0x82240270,
    0x82240338, 0x82240430, 0x82240698, 0x822406A8, 0x82240728, 0x82240748,
    0x822407E0, 0x82240B48, 0x82240E20, 0x82240E28, 0x82240E48, 0x82240E70,
    0x822410D8, 0x82241318, 0x822413B8, 0x82241410, 0x822414B8, 0x82241530,
    0x822415A0, 0x822417C0, 0x82241820, 0x822419E0, 0x82241F28, 0x822420F8,
    0x82242210, 0x82242270, 0x822423A8, 0x822423B8, 0x822424A0, 0x82242588,
    0x822425E8, 0x82242648, 0x822426A0, 0x822426F8, 0x822428B8, 0x82242900,
    0x822429F8, 0x82242BC8, 0x82242C10, 0x82242C28, 0x82242D18, 0x82242DA0,
    0x82242E88, 0x822430D0, 0x82243118, 0x82243168, 0x82243218, 0x822432C8,
    0x82243468, 0x822435F0, 0x822436D0, 0x822437C0, 0x82243810, 0x82243818,
    0x82243880, 0x82243888, 0x822439F8, 0x82243A08, 0x82243A78, 0x82243AA8,
    0x82243BC0, 0x82244088, 0x82244110, 0x82244168, 0x82244558, 0x82244850,
    0x82244A10, 0x82244B28, 0x82244BB8, 0x82244C98, 0x82244F40, 0x82245068,
    0x822450E0, 0x82245140, 0x822451D8, 0x822452D8, 0x82245508, 0x82245590,
    0x822459E8, 0x82245B60, 0x82245BB8, 0x82245CB0, 0x82245DA8, 0x82246228,
    0x82246508, 0x82246A50, 0x82246C20, 0x82246CA0, 0x82246CD0, 0x82246DB8,
    0x82246DD0, 0x82246DE8, 0x82246E20, 0x82246E78, 0x82246EC0, 0x82246ED8,
    0x82246EF0, 0x82246F18, 0x822471D8, 0x82247258, 0x82247368, 0x82247A98,
    0x82247B50, 0x82247F20, 0x822487A0, 0x822487D0, 0x82248B70, 0x82248C60,
    0x82248CB0, 0x82248D58, 0x82248DC0, 0x82248ED8, 0x82248F34, 0x82249268,
    0x82249340, 0x8224A188, 0x8224A1A0, 0x8224A1C8, 0x8224A238, 0x8224A320,
    0x8224A3B8, 0x8224A9C0, 0x8224A9D8, 0x8224C010, 0x8224C078, 0x8224C0E0,
    0x8224C2B0, 0x8224C328, 0x8224C410, 0x8224C478, 0x8224C578, 0x8224C5D8,
    0x8224C778, 0x8224C7D8, 0x8224C860, 0x8224C960, 0x8224CB00, 0x8224CC48,
    0x8224CF80, 0x8224D0A8, 0x8224D3C0, 0x8224D4B0, 0x8224D538, 0x8224D588,
    0x8224D6D8, 0x8224D768, 0x8224DA58, 0x8224DCB8, 0x8224E220, 0x8224E8B0,
    0x8224E9D8, 0x8224EA98, 0x8224EAF8, 0x8224EB18, 0x8224EBD8, 0x8224ECB0,
    0x8224EF70, 0x8224F280, 0x8224F320, 0x8224F3B0, 0x8224F3B8, 0x8224F458,
    0x8224F518, 0x8224F5A8, 0x8224F600, 0x8224F658, 0x8224F6B8, 0x8224F728,
    0x8224F8E8, 0x8224F958, 0x8224F9C8, 0x8224FA30, 0x8224FAA0, 0x8224FD00,
    0x8224FD08, 0x8224FD78, 0x8224FDF0, 0x8224FE78, 0x8224FF40, 0x82250038,
    0x82250198, 0x82250248, 0x822502A0, 0x822505A8, 0x82250690, 0x822506B8,
    0x822506E8, 0x82250740, 0x82250778, 0x82250788, 0x822507F0, 0x82250800,
    0x82250850, 0x822508D0, 0x82250940, 0x82250998, 0x82250A68, 0x82250AC0,
    0x82250B30, 0x82250BA0, 0x82250D00, 0x82250D58, 0x82250E90, 0x82250F18,
    0x82251028, 0x82251190, 0x822512E8, 0x822513A8, 0x822515F8, 0x822516E8,
    0x822517F8, 0x82251890, 0x82251910, 0x822519A0, 0x82251A28, 0x82251C08,
    0x82251C90, 0x82251D48, 0x82251DB8, 0x82251E28, 0x82251EB0, 0x82251F10,
    0x82251F98, 0x82251FF0, 0x82252060, 0x82252070, 0x82252080, 0x82252088,
    0x82252090, 0x82252108, 0x82252110, 0x822521B0, 0x822521B8, 0x82252288,
    0x82252320, 0x82252410, 0x82252418, 0x82252420, 0x82252470, 0x822524F0,
    0x82252560, 0x822526A8, 0x82252AB8, 0x82252C88, 0x82252CE8, 0x82252D50,
    0x82252F98, 0x82252FA0, 0x82252FA8, 0x822530A0, 0x822533D8, 0x82253468,
    0x82253528, 0x82253580, 0x822535D8, 0x82253798, 0x822538A0, 0x822538F8,
    0x82253BB0, 0x82253C40, 0x82253E68, 0x82253EC8, 0x822540A0, 0x82254130,
    0x82254278, 0x82254300, 0x822543A8, 0x822543B0, 0x82254478, 0x822544D0,
    0x822545A8, 0x82254758, 0x822547E8, 0x82254878, 0x82254930, 0x82254940,
    0x82254A30, 0x82254A88, 0x82254A90, 0x82254A98, 0x82254AF0, 0x82254B78,
    0x82254C08, 0x82254CD8, 0x82254E70, 0x82254F10, 0x82254F18, 0x822550A8,
    0x82255118, 0x82255178, 0x822551D0, 0x82255350, 0x82255380, 0x822554C8,
    0x82255740, 0x82255948, 0x82255AF8, 0x82255DF0, 0x82255E68, 0x82255EE0,
    0x82255EF8, 0x82255F98, 0x822560B8, 0x822560C0, 0x822560C8, 0x822561A8,
    0x82256208, 0x82256498, 0x822565E8, 0x82256670, 0x822566F0, 0x82256780,
    0x822567F0, 0x82256898, 0x822568F0, 0x82256A90, 0x82256B38, 0x82256BC8,
    0x82256CD0, 0x82256CF8, 0x82256D90, 0x82256E00, 0x82256E70, 0x82256F20,
    0x82256F28, 0x82256F30, 0x82256F80, 0x82256FC0, 0x82257258, 0x82257500,
    0x82257618, 0x82257678, 0x82257860, 0x822578B8, 0x82257938, 0x82257A10,
    0x82257A28, 0x82257A40, 0x82257A50, 0x82257A58, 0x82257A60, 0x82257AF0,
    0x82257B70, 0x82257BC8, 0x82257BD0, 0x82257BD8, 0x82257CF0, 0x82257CF8,
    0x82257D00, 0x82257D28, 0x82257DA0, 0x82257EF8, 0x825AA0E0, 0x825AA2E0,
    0x825AA360, 0x825AA3D0, 0x825AA4D0, 0x825AA8D0, 0x825AA988, 0x825AA9F0,
    0x825AAA00, 0x825AAC58, 0x825AAE90, 0x825AAE98, 0x825AAF28, 0x825AB3C8,
    0x825AB3E0, 0x825AB3E8, 0x825AB7D0, 0x825ABC28, 0x825ABC70, 0x825ABCA0,
    0x825ABD00,
};

static svr_census::Census g_census(kCensusAddrs, 847, 217);

REX_HOOK_RAW(sub_8222E0E8) { g_census.Hit(0, ctx); __imp__sub_8222E0E8(ctx, base); }
REX_HOOK_RAW(sub_8222E278) { g_census.Hit(1, ctx); __imp__sub_8222E278(ctx, base); }
REX_HOOK_RAW(sub_8222E3A8) { g_census.Hit(2, ctx); __imp__sub_8222E3A8(ctx, base); }
REX_HOOK_RAW(sub_8222E498) { g_census.Hit(3, ctx); __imp__sub_8222E498(ctx, base); }
REX_HOOK_RAW(sub_8222E588) { g_census.Hit(4, ctx); __imp__sub_8222E588(ctx, base); }
REX_HOOK_RAW(sub_8222E630) { g_census.Hit(5, ctx); __imp__sub_8222E630(ctx, base); }
REX_HOOK_RAW(sub_8222E6A0) { g_census.Hit(6, ctx); __imp__sub_8222E6A0(ctx, base); }
REX_HOOK_RAW(sub_8222E7A0) { g_census.Hit(7, ctx); __imp__sub_8222E7A0(ctx, base); }
REX_HOOK_RAW(sub_8222E828) { g_census.Hit(8, ctx); __imp__sub_8222E828(ctx, base); }
REX_HOOK_RAW(sub_8222E888) { g_census.Hit(9, ctx); __imp__sub_8222E888(ctx, base); }
REX_HOOK_RAW(sub_8222E980) { g_census.Hit(10, ctx); __imp__sub_8222E980(ctx, base); }
REX_HOOK_RAW(sub_8222EA18) { g_census.Hit(11, ctx); __imp__sub_8222EA18(ctx, base); }
REX_HOOK_RAW(sub_8222EA88) { g_census.Hit(12, ctx); __imp__sub_8222EA88(ctx, base); }
REX_HOOK_RAW(sub_8222EBC0) { g_census.Hit(13, ctx); __imp__sub_8222EBC0(ctx, base); }
REX_HOOK_RAW(sub_8222EC68) { g_census.Hit(14, ctx); __imp__sub_8222EC68(ctx, base); }
REX_HOOK_RAW(sub_8222F068) { g_census.Hit(15, ctx); __imp__sub_8222F068(ctx, base); }
REX_HOOK_RAW(sub_8222F168) { g_census.Hit(16, ctx); __imp__sub_8222F168(ctx, base); }
REX_HOOK_RAW(sub_8222F480) { g_census.Hit(17, ctx); __imp__sub_8222F480(ctx, base); }
REX_HOOK_RAW(sub_8222F508) { g_census.Hit(18, ctx); __imp__sub_8222F508(ctx, base); }
REX_HOOK_RAW(sub_8222F578) { g_census.Hit(19, ctx); __imp__sub_8222F578(ctx, base); }
REX_HOOK_RAW(sub_8222F580) { g_census.Hit(20, ctx); __imp__sub_8222F580(ctx, base); }
REX_HOOK_RAW(sub_8222F590) { g_census.Hit(21, ctx); __imp__sub_8222F590(ctx, base); }
REX_HOOK_RAW(sub_8222F598) { g_census.Hit(22, ctx); __imp__sub_8222F598(ctx, base); }
REX_HOOK_RAW(sub_8222F630) { g_census.Hit(23, ctx); __imp__sub_8222F630(ctx, base); }
REX_HOOK_RAW(sub_8222F648) { g_census.Hit(24, ctx); __imp__sub_8222F648(ctx, base); }
REX_HOOK_RAW(sub_8222F650) { g_census.Hit(25, ctx); __imp__sub_8222F650(ctx, base); }
REX_HOOK_RAW(sub_8222F660) { g_census.Hit(26, ctx); __imp__sub_8222F660(ctx, base); }
REX_HOOK_RAW(sub_8222F810) { g_census.Hit(27, ctx); __imp__sub_8222F810(ctx, base); }
REX_HOOK_RAW(sub_8222F848) { g_census.Hit(28, ctx); __imp__sub_8222F848(ctx, base); }
REX_HOOK_RAW(sub_8222F868) { g_census.Hit(29, ctx); __imp__sub_8222F868(ctx, base); }
REX_HOOK_RAW(sub_8222F8B0) { g_census.Hit(30, ctx); __imp__sub_8222F8B0(ctx, base); }
REX_HOOK_RAW(sub_8222F8B8) { g_census.Hit(31, ctx); __imp__sub_8222F8B8(ctx, base); }
REX_HOOK_RAW(sub_8222F958) { g_census.Hit(32, ctx); __imp__sub_8222F958(ctx, base); }
REX_HOOK_RAW(sub_8222F998) { g_census.Hit(33, ctx); __imp__sub_8222F998(ctx, base); }
REX_HOOK_RAW(sub_8222FA18) { g_census.Hit(34, ctx); __imp__sub_8222FA18(ctx, base); }
REX_HOOK_RAW(sub_8222FA58) { g_census.Hit(35, ctx); __imp__sub_8222FA58(ctx, base); }
REX_HOOK_RAW(sub_8222FA70) { g_census.Hit(36, ctx); __imp__sub_8222FA70(ctx, base); }
REX_HOOK_RAW(sub_8222FB20) { g_census.Hit(37, ctx); __imp__sub_8222FB20(ctx, base); }
REX_HOOK_RAW(sub_8222FB28) { g_census.Hit(38, ctx); __imp__sub_8222FB28(ctx, base); }
REX_HOOK_RAW(sub_8222FBB8) { g_census.Hit(39, ctx); __imp__sub_8222FBB8(ctx, base); }
REX_HOOK_RAW(sub_8222FBC8) { g_census.Hit(40, ctx); __imp__sub_8222FBC8(ctx, base); }
REX_HOOK_RAW(sub_8222FBD8) { g_census.Hit(41, ctx); __imp__sub_8222FBD8(ctx, base); }
REX_HOOK_RAW(sub_8222FC10) { g_census.Hit(42, ctx); __imp__sub_8222FC10(ctx, base); }
REX_HOOK_RAW(sub_8222FC18) { g_census.Hit(43, ctx); __imp__sub_8222FC18(ctx, base); }
REX_HOOK_RAW(sub_8222FC28) { g_census.Hit(44, ctx); __imp__sub_8222FC28(ctx, base); }
REX_HOOK_RAW(sub_8222FC50) { g_census.Hit(45, ctx); __imp__sub_8222FC50(ctx, base); }
REX_HOOK_RAW(sub_8222FC90) { g_census.Hit(46, ctx); __imp__sub_8222FC90(ctx, base); }
REX_HOOK_RAW(sub_8222FD20) { g_census.Hit(47, ctx); __imp__sub_8222FD20(ctx, base); }
REX_HOOK_RAW(sub_8222FDD8) { g_census.Hit(48, ctx); __imp__sub_8222FDD8(ctx, base); }
REX_HOOK_RAW(sub_8222FE60) { g_census.Hit(49, ctx); __imp__sub_8222FE60(ctx, base); }
REX_HOOK_RAW(sub_82230208) { g_census.Hit(50, ctx); __imp__sub_82230208(ctx, base); }
REX_HOOK_RAW(sub_82230230) { g_census.Hit(51, ctx); __imp__sub_82230230(ctx, base); }
REX_HOOK_RAW(sub_822302F0) { g_census.Hit(52, ctx); __imp__sub_822302F0(ctx, base); }
REX_HOOK_RAW(sub_82230614) { g_census.Hit(53, ctx); __imp__sub_82230614(ctx, base); }
REX_HOOK_RAW(sub_82230AE0) { g_census.Hit(54, ctx); __imp__sub_82230AE0(ctx, base); }
REX_HOOK_RAW(sub_82230E70) { g_census.Hit(55, ctx); __imp__sub_82230E70(ctx, base); }
REX_HOOK_RAW(sub_82231390) { g_census.Hit(56, ctx); __imp__sub_82231390(ctx, base); }
REX_HOOK_RAW(sub_82231398) { g_census.Hit(57, ctx); __imp__sub_82231398(ctx, base); }
REX_HOOK_RAW(sub_822313E0) { g_census.Hit(58, ctx); __imp__sub_822313E0(ctx, base); }
REX_HOOK_RAW(sub_82231428) { g_census.Hit(59, ctx); __imp__sub_82231428(ctx, base); }
REX_HOOK_RAW(sub_82231460) { g_census.Hit(60, ctx); __imp__sub_82231460(ctx, base); }
REX_HOOK_RAW(sub_82231478) { g_census.Hit(61, ctx); __imp__sub_82231478(ctx, base); }
REX_HOOK_RAW(sub_822314B0) { g_census.Hit(62, ctx); __imp__sub_822314B0(ctx, base); }
REX_HOOK_RAW(sub_822314D0) { g_census.Hit(63, ctx); __imp__sub_822314D0(ctx, base); }
REX_HOOK_RAW(sub_82231538) { g_census.Hit(64, ctx); __imp__sub_82231538(ctx, base); }
REX_HOOK_RAW(sub_82231590) { g_census.Hit(65, ctx); __imp__sub_82231590(ctx, base); }
REX_HOOK_RAW(sub_82231598) { g_census.Hit(66, ctx); __imp__sub_82231598(ctx, base); }
REX_HOOK_RAW(sub_82231608) { g_census.Hit(67, ctx); __imp__sub_82231608(ctx, base); }
REX_HOOK_RAW(sub_82231780) { g_census.Hit(68, ctx); __imp__sub_82231780(ctx, base); }
REX_HOOK_RAW(sub_822317E0) { g_census.Hit(69, ctx); __imp__sub_822317E0(ctx, base); }
REX_HOOK_RAW(sub_82231870) { g_census.Hit(70, ctx); __imp__sub_82231870(ctx, base); }
REX_HOOK_RAW(sub_822318D0) { g_census.Hit(71, ctx); __imp__sub_822318D0(ctx, base); }
REX_HOOK_RAW(sub_82231940) { g_census.Hit(72, ctx); __imp__sub_82231940(ctx, base); }
REX_HOOK_RAW(sub_822319D8) { g_census.Hit(73, ctx); __imp__sub_822319D8(ctx, base); }
REX_HOOK_RAW(sub_822319E8) { g_census.Hit(74, ctx); __imp__sub_822319E8(ctx, base); }
REX_HOOK_RAW(sub_82231AC8) { g_census.Hit(75, ctx); __imp__sub_82231AC8(ctx, base); }
REX_HOOK_RAW(sub_82231B40) { g_census.Hit(76, ctx); __imp__sub_82231B40(ctx, base); }
REX_HOOK_RAW(sub_82231C18) { g_census.Hit(77, ctx); __imp__sub_82231C18(ctx, base); }
REX_HOOK_RAW(sub_82231C70) { g_census.Hit(78, ctx); __imp__sub_82231C70(ctx, base); }
REX_HOOK_RAW(sub_82231E30) { g_census.Hit(79, ctx); __imp__sub_82231E30(ctx, base); }
REX_HOOK_RAW(sub_82231F50) { g_census.Hit(80, ctx); __imp__sub_82231F50(ctx, base); }
REX_HOOK_RAW(sub_822321E0) { g_census.Hit(81, ctx); __imp__sub_822321E0(ctx, base); }
REX_HOOK_RAW(sub_822325E8) { g_census.Hit(82, ctx); __imp__sub_822325E8(ctx, base); }
REX_HOOK_RAW(sub_82232720) { g_census.Hit(83, ctx); __imp__sub_82232720(ctx, base); }
REX_HOOK_RAW(sub_82232C90) { g_census.Hit(84, ctx); __imp__sub_82232C90(ctx, base); }
REX_HOOK_RAW(sub_82232D38) { g_census.Hit(85, ctx); __imp__sub_82232D38(ctx, base); }
REX_HOOK_RAW(sub_82232EA0) { g_census.Hit(86, ctx); __imp__sub_82232EA0(ctx, base); }
REX_HOOK_RAW(sub_822330E0) { g_census.Hit(87, ctx); __imp__sub_822330E0(ctx, base); }
REX_HOOK_RAW(sub_82233448) { g_census.Hit(88, ctx); __imp__sub_82233448(ctx, base); }
REX_HOOK_RAW(sub_822334E0) { g_census.Hit(89, ctx); __imp__sub_822334E0(ctx, base); }
REX_HOOK_RAW(sub_822339E0) { g_census.Hit(90, ctx); __imp__sub_822339E0(ctx, base); }
REX_HOOK_RAW(sub_822339F8) { g_census.Hit(91, ctx); __imp__sub_822339F8(ctx, base); }
REX_HOOK_RAW(sub_82234264) { g_census.Hit(92, ctx); __imp__sub_82234264(ctx, base); }
REX_HOOK_RAW(sub_8223428C) { g_census.Hit(93, ctx); __imp__sub_8223428C(ctx, base); }
REX_HOOK_RAW(sub_822342E0) { g_census.Hit(94, ctx); __imp__sub_822342E0(ctx, base); }
REX_HOOK_RAW(sub_8223454C) { g_census.Hit(95, ctx); __imp__sub_8223454C(ctx, base); }
REX_HOOK_RAW(sub_82234574) { g_census.Hit(96, ctx); __imp__sub_82234574(ctx, base); }
REX_HOOK_RAW(sub_822345C8) { g_census.Hit(97, ctx); __imp__sub_822345C8(ctx, base); }
REX_HOOK_RAW(sub_82234C48) { g_census.Hit(98, ctx); __imp__sub_82234C48(ctx, base); }
REX_HOOK_RAW(sub_82234DC0) { g_census.Hit(99, ctx); __imp__sub_82234DC0(ctx, base); }
REX_HOOK_RAW(sub_82234DE4) { g_census.Hit(100, ctx); __imp__sub_82234DE4(ctx, base); }
REX_HOOK_RAW(sub_82234E04) { g_census.Hit(101, ctx); __imp__sub_82234E04(ctx, base); }
REX_HOOK_RAW(sub_82234E48) { g_census.Hit(102, ctx); __imp__sub_82234E48(ctx, base); }
REX_HOOK_RAW(sub_82234E70) { g_census.Hit(103, ctx); __imp__sub_82234E70(ctx, base); }
REX_HOOK_RAW(sub_82234E80) { g_census.Hit(104, ctx); __imp__sub_82234E80(ctx, base); }
REX_HOOK_RAW(sub_82234E90) { g_census.Hit(105, ctx); __imp__sub_82234E90(ctx, base); }
REX_HOOK_RAW(sub_82234EB8) { g_census.Hit(106, ctx); __imp__sub_82234EB8(ctx, base); }
REX_HOOK_RAW(sub_82234F14) { g_census.Hit(107, ctx); __imp__sub_82234F14(ctx, base); }
REX_HOOK_RAW(sub_82234F38) { g_census.Hit(108, ctx); __imp__sub_82234F38(ctx, base); }
REX_HOOK_RAW(sub_82234F40) { g_census.Hit(109, ctx); __imp__sub_82234F40(ctx, base); }
REX_HOOK_RAW(sub_82234FD0) { g_census.Hit(110, ctx); __imp__sub_82234FD0(ctx, base); }
REX_HOOK_RAW(sub_82234FD8) { g_census.Hit(111, ctx); __imp__sub_82234FD8(ctx, base); }
REX_HOOK_RAW(sub_82235030) { g_census.Hit(112, ctx); __imp__sub_82235030(ctx, base); }
REX_HOOK_RAW(sub_822351C0) { g_census.Hit(113, ctx); __imp__sub_822351C0(ctx, base); }
REX_HOOK_RAW(sub_822353F8) { g_census.Hit(114, ctx); __imp__sub_822353F8(ctx, base); }
REX_HOOK_RAW(sub_82235500) { g_census.Hit(115, ctx); __imp__sub_82235500(ctx, base); }
REX_HOOK_RAW(sub_82235578) { g_census.Hit(116, ctx); __imp__sub_82235578(ctx, base); }
REX_HOOK_RAW(sub_822355F8) { g_census.Hit(117, ctx); __imp__sub_822355F8(ctx, base); }
REX_HOOK_RAW(sub_82235648) { g_census.Hit(118, ctx); __imp__sub_82235648(ctx, base); }
REX_HOOK_RAW(sub_82235710) { g_census.Hit(119, ctx); __imp__sub_82235710(ctx, base); }
REX_HOOK_RAW(sub_82235760) { g_census.Hit(120, ctx); __imp__sub_82235760(ctx, base); }
REX_HOOK_RAW(sub_82235770) { g_census.Hit(121, ctx); __imp__sub_82235770(ctx, base); }
REX_HOOK_RAW(sub_82235820) { g_census.Hit(122, ctx); __imp__sub_82235820(ctx, base); }
REX_HOOK_RAW(sub_82235868) { g_census.Hit(123, ctx); __imp__sub_82235868(ctx, base); }
REX_HOOK_RAW(sub_82235878) { g_census.Hit(124, ctx); __imp__sub_82235878(ctx, base); }
REX_HOOK_RAW(sub_82235910) { g_census.Hit(125, ctx); __imp__sub_82235910(ctx, base); }
REX_HOOK_RAW(sub_82235928) { g_census.Hit(126, ctx); __imp__sub_82235928(ctx, base); }
REX_HOOK_RAW(sub_82235940) { g_census.Hit(127, ctx); __imp__sub_82235940(ctx, base); }
REX_HOOK_RAW(sub_822359B0) { g_census.Hit(128, ctx); __imp__sub_822359B0(ctx, base); }
REX_HOOK_RAW(sub_82235A48) { g_census.Hit(129, ctx); __imp__sub_82235A48(ctx, base); }
REX_HOOK_RAW(sub_82235AD0) { g_census.Hit(130, ctx); __imp__sub_82235AD0(ctx, base); }
REX_HOOK_RAW(sub_82235BF0) { g_census.Hit(131, ctx); __imp__sub_82235BF0(ctx, base); }
REX_HOOK_RAW(sub_82235F60) { g_census.Hit(132, ctx); __imp__sub_82235F60(ctx, base); }
REX_HOOK_RAW(sub_822360B8) { g_census.Hit(133, ctx); __imp__sub_822360B8(ctx, base); }
REX_HOOK_RAW(sub_82236480) { g_census.Hit(134, ctx); __imp__sub_82236480(ctx, base); }
REX_HOOK_RAW(sub_82236580) { g_census.Hit(135, ctx); __imp__sub_82236580(ctx, base); }
REX_HOOK_RAW(sub_82236680) { g_census.Hit(136, ctx); __imp__sub_82236680(ctx, base); }
REX_HOOK_RAW(sub_82236B40) { g_census.Hit(137, ctx); __imp__sub_82236B40(ctx, base); }
REX_HOOK_RAW(sub_82236C08) { g_census.Hit(138, ctx); __imp__sub_82236C08(ctx, base); }
REX_HOOK_RAW(sub_82236C98) { g_census.Hit(139, ctx); __imp__sub_82236C98(ctx, base); }
REX_HOOK_RAW(sub_82236D40) { g_census.Hit(140, ctx); __imp__sub_82236D40(ctx, base); }
REX_HOOK_RAW(sub_82237050) { g_census.Hit(141, ctx); __imp__sub_82237050(ctx, base); }
REX_HOOK_RAW(sub_822372A8) { g_census.Hit(142, ctx); __imp__sub_822372A8(ctx, base); }
REX_HOOK_RAW(sub_822372B8) { g_census.Hit(143, ctx); __imp__sub_822372B8(ctx, base); }
REX_HOOK_RAW(sub_82237370) { g_census.Hit(144, ctx); __imp__sub_82237370(ctx, base); }
REX_HOOK_RAW(sub_82237388) { g_census.Hit(145, ctx); __imp__sub_82237388(ctx, base); }
REX_HOOK_RAW(sub_82237440) { g_census.Hit(146, ctx); __imp__sub_82237440(ctx, base); }
REX_HOOK_RAW(sub_82237448) { g_census.Hit(147, ctx); __imp__sub_82237448(ctx, base); }
REX_HOOK_RAW(sub_822374F8) { g_census.Hit(148, ctx); __imp__sub_822374F8(ctx, base); }
REX_HOOK_RAW(sub_82237500) { g_census.Hit(149, ctx); __imp__sub_82237500(ctx, base); }
REX_HOOK_RAW(sub_82237620) { g_census.Hit(150, ctx); __imp__sub_82237620(ctx, base); }
REX_HOOK_RAW(sub_82237748) { g_census.Hit(151, ctx); __imp__sub_82237748(ctx, base); }
REX_HOOK_RAW(sub_822377E0) { g_census.Hit(152, ctx); __imp__sub_822377E0(ctx, base); }
REX_HOOK_RAW(sub_82237800) { g_census.Hit(153, ctx); __imp__sub_82237800(ctx, base); }
REX_HOOK_RAW(sub_82237848) { g_census.Hit(154, ctx); __imp__sub_82237848(ctx, base); }
REX_HOOK_RAW(sub_82237868) { g_census.Hit(155, ctx); __imp__sub_82237868(ctx, base); }
REX_HOOK_RAW(sub_822379E0) { g_census.Hit(156, ctx); __imp__sub_822379E0(ctx, base); }
REX_HOOK_RAW(sub_82237A78) { g_census.Hit(157, ctx); __imp__sub_82237A78(ctx, base); }
REX_HOOK_RAW(sub_82237B28) { g_census.Hit(158, ctx); __imp__sub_82237B28(ctx, base); }
REX_HOOK_RAW(sub_82237C18) { g_census.Hit(159, ctx); __imp__sub_82237C18(ctx, base); }
REX_HOOK_RAW(sub_82237C88) { g_census.Hit(160, ctx); __imp__sub_82237C88(ctx, base); }
REX_HOOK_RAW(sub_82237CE0) { g_census.Hit(161, ctx); __imp__sub_82237CE0(ctx, base); }
REX_HOOK_RAW(sub_82237D98) { g_census.Hit(162, ctx); __imp__sub_82237D98(ctx, base); }
REX_HOOK_RAW(sub_82237E38) { g_census.Hit(163, ctx); __imp__sub_82237E38(ctx, base); }
REX_HOOK_RAW(sub_82237E60) { g_census.Hit(164, ctx); __imp__sub_82237E60(ctx, base); }
REX_HOOK_RAW(sub_82238068) { g_census.Hit(165, ctx); __imp__sub_82238068(ctx, base); }
REX_HOOK_RAW(sub_82238150) { g_census.Hit(166, ctx); __imp__sub_82238150(ctx, base); }
REX_HOOK_RAW(sub_82238338) { g_census.Hit(167, ctx); __imp__sub_82238338(ctx, base); }
REX_HOOK_RAW(sub_822384F8) { g_census.Hit(168, ctx); __imp__sub_822384F8(ctx, base); }
REX_HOOK_RAW(sub_822385E0) { g_census.Hit(169, ctx); __imp__sub_822385E0(ctx, base); }
REX_HOOK_RAW(sub_822386B8) { g_census.Hit(170, ctx); __imp__sub_822386B8(ctx, base); }
REX_HOOK_RAW(sub_82238780) { g_census.Hit(171, ctx); __imp__sub_82238780(ctx, base); }
REX_HOOK_RAW(sub_822388F0) { g_census.Hit(172, ctx); __imp__sub_822388F0(ctx, base); }
REX_HOOK_RAW(sub_82238910) { g_census.Hit(173, ctx); __imp__sub_82238910(ctx, base); }
REX_HOOK_RAW(sub_82238930) { g_census.Hit(174, ctx); __imp__sub_82238930(ctx, base); }
REX_HOOK_RAW(sub_82238A28) { g_census.Hit(175, ctx); __imp__sub_82238A28(ctx, base); }
REX_HOOK_RAW(sub_82238B80) { g_census.Hit(176, ctx); __imp__sub_82238B80(ctx, base); }
REX_HOOK_RAW(sub_82238D08) { g_census.Hit(177, ctx); __imp__sub_82238D08(ctx, base); }
REX_HOOK_RAW(sub_82238E08) { g_census.Hit(178, ctx); __imp__sub_82238E08(ctx, base); }
REX_HOOK_RAW(sub_82238E40) { g_census.Hit(179, ctx); __imp__sub_82238E40(ctx, base); }
REX_HOOK_RAW(sub_82238EB8) { g_census.Hit(180, ctx); __imp__sub_82238EB8(ctx, base); }
REX_HOOK_RAW(sub_82238F20) { g_census.Hit(181, ctx); __imp__sub_82238F20(ctx, base); }
REX_HOOK_RAW(sub_82238F88) { g_census.Hit(182, ctx); __imp__sub_82238F88(ctx, base); }
REX_HOOK_RAW(sub_82239000) { g_census.Hit(183, ctx); __imp__sub_82239000(ctx, base); }
REX_HOOK_RAW(sub_82239058) { g_census.Hit(184, ctx); __imp__sub_82239058(ctx, base); }
REX_HOOK_RAW(sub_82239468) { g_census.Hit(185, ctx); __imp__sub_82239468(ctx, base); }
REX_HOOK_RAW(sub_82239518) { g_census.Hit(186, ctx); __imp__sub_82239518(ctx, base); }
REX_HOOK_RAW(sub_82239570) { g_census.Hit(187, ctx); __imp__sub_82239570(ctx, base); }
REX_HOOK_RAW(sub_82239578) { g_census.Hit(188, ctx); __imp__sub_82239578(ctx, base); }
REX_HOOK_RAW(sub_82239620) { g_census.Hit(189, ctx); __imp__sub_82239620(ctx, base); }
REX_HOOK_RAW(sub_822396C8) { g_census.Hit(190, ctx); __imp__sub_822396C8(ctx, base); }
REX_HOOK_RAW(sub_82239748) { g_census.Hit(191, ctx); __imp__sub_82239748(ctx, base); }
REX_HOOK_RAW(sub_822397A8) { g_census.Hit(192, ctx); __imp__sub_822397A8(ctx, base); }
REX_HOOK_RAW(sub_822397F8) { g_census.Hit(193, ctx); __imp__sub_822397F8(ctx, base); }
REX_HOOK_RAW(sub_822398D8) { g_census.Hit(194, ctx); __imp__sub_822398D8(ctx, base); }
REX_HOOK_RAW(sub_82239A10) { g_census.Hit(195, ctx); __imp__sub_82239A10(ctx, base); }
REX_HOOK_RAW(sub_82239A60) { g_census.Hit(196, ctx); __imp__sub_82239A60(ctx, base); }
REX_HOOK_RAW(sub_82239BD8) { g_census.Hit(197, ctx); __imp__sub_82239BD8(ctx, base); }
REX_HOOK_RAW(sub_82239CB8) { g_census.Hit(198, ctx); __imp__sub_82239CB8(ctx, base); }
REX_HOOK_RAW(sub_82239CE8) { g_census.Hit(199, ctx); __imp__sub_82239CE8(ctx, base); }
REX_HOOK_RAW(sub_82239DA8) { g_census.Hit(200, ctx); __imp__sub_82239DA8(ctx, base); }
REX_HOOK_RAW(sub_82239DC0) { g_census.Hit(201, ctx); __imp__sub_82239DC0(ctx, base); }
REX_HOOK_RAW(sub_82239E18) { g_census.Hit(202, ctx); __imp__sub_82239E18(ctx, base); }
REX_HOOK_RAW(sub_82239E68) { g_census.Hit(203, ctx); __imp__sub_82239E68(ctx, base); }
REX_HOOK_RAW(sub_82239F40) { g_census.Hit(204, ctx); __imp__sub_82239F40(ctx, base); }
REX_HOOK_RAW(sub_82239F98) { g_census.Hit(205, ctx); __imp__sub_82239F98(ctx, base); }
REX_HOOK_RAW(sub_82239FD8) { g_census.Hit(206, ctx); __imp__sub_82239FD8(ctx, base); }
REX_HOOK_RAW(sub_82239FE8) { g_census.Hit(207, ctx); __imp__sub_82239FE8(ctx, base); }
REX_HOOK_RAW(sub_8223A0F8) { g_census.Hit(208, ctx); __imp__sub_8223A0F8(ctx, base); }
REX_HOOK_RAW(sub_8223A1B0) { g_census.Hit(209, ctx); __imp__sub_8223A1B0(ctx, base); }
REX_HOOK_RAW(sub_8223A220) { g_census.Hit(210, ctx); __imp__sub_8223A220(ctx, base); }
REX_HOOK_RAW(sub_8223A320) { g_census.Hit(211, ctx); __imp__sub_8223A320(ctx, base); }
REX_HOOK_RAW(sub_8223A4B8) { g_census.Hit(212, ctx); __imp__sub_8223A4B8(ctx, base); }
REX_HOOK_RAW(sub_8223A660) { g_census.Hit(213, ctx); __imp__sub_8223A660(ctx, base); }
REX_HOOK_RAW(sub_8223A700) { g_census.Hit(214, ctx); __imp__sub_8223A700(ctx, base); }
REX_HOOK_RAW(sub_8223A780) { g_census.Hit(215, ctx); __imp__sub_8223A780(ctx, base); }
REX_HOOK_RAW(sub_8223A958) { g_census.Hit(216, ctx); __imp__sub_8223A958(ctx, base); }
REX_HOOK_RAW(sub_8223A960) { g_census.Hit(217, ctx); __imp__sub_8223A960(ctx, base); }
REX_HOOK_RAW(sub_8223AEF8) { g_census.Hit(218, ctx); __imp__sub_8223AEF8(ctx, base); }
REX_HOOK_RAW(sub_8223B028) { g_census.Hit(219, ctx); __imp__sub_8223B028(ctx, base); }
REX_HOOK_RAW(sub_8223B088) { g_census.Hit(220, ctx); __imp__sub_8223B088(ctx, base); }
REX_HOOK_RAW(sub_8223B098) { g_census.Hit(221, ctx); __imp__sub_8223B098(ctx, base); }
REX_HOOK_RAW(sub_8223B0B0) { g_census.Hit(222, ctx); __imp__sub_8223B0B0(ctx, base); }
REX_HOOK_RAW(sub_8223B130) { g_census.Hit(223, ctx); __imp__sub_8223B130(ctx, base); }
REX_HOOK_RAW(sub_8223B398) { g_census.Hit(224, ctx); __imp__sub_8223B398(ctx, base); }
REX_HOOK_RAW(sub_8223B468) { g_census.Hit(225, ctx); __imp__sub_8223B468(ctx, base); }
REX_HOOK_RAW(sub_8223B6E8) { g_census.Hit(226, ctx); __imp__sub_8223B6E8(ctx, base); }
REX_HOOK_RAW(sub_8223B788) { g_census.Hit(227, ctx); __imp__sub_8223B788(ctx, base); }
REX_HOOK_RAW(sub_8223B840) { g_census.Hit(228, ctx); __imp__sub_8223B840(ctx, base); }
REX_HOOK_RAW(sub_8223BA40) { g_census.Hit(229, ctx); __imp__sub_8223BA40(ctx, base); }
REX_HOOK_RAW(sub_8223BDD8) { g_census.Hit(230, ctx); __imp__sub_8223BDD8(ctx, base); }
REX_HOOK_RAW(sub_8223BE98) { g_census.Hit(231, ctx); __imp__sub_8223BE98(ctx, base); }
REX_HOOK_RAW(sub_8223C018) { g_census.Hit(232, ctx); __imp__sub_8223C018(ctx, base); }
REX_HOOK_RAW(sub_8223C178) { g_census.Hit(233, ctx); __imp__sub_8223C178(ctx, base); }
REX_HOOK_RAW(sub_8223C2D8) { g_census.Hit(234, ctx); __imp__sub_8223C2D8(ctx, base); }
REX_HOOK_RAW(sub_8223C8E8) { g_census.Hit(235, ctx); __imp__sub_8223C8E8(ctx, base); }
REX_HOOK_RAW(sub_8223C9B8) { g_census.Hit(236, ctx); __imp__sub_8223C9B8(ctx, base); }
REX_HOOK_RAW(sub_8223CA70) { g_census.Hit(237, ctx); __imp__sub_8223CA70(ctx, base); }
REX_HOOK_RAW(sub_8223CBC0) { g_census.Hit(238, ctx); __imp__sub_8223CBC0(ctx, base); }
REX_HOOK_RAW(sub_8223CD20) { g_census.Hit(239, ctx); __imp__sub_8223CD20(ctx, base); }
REX_HOOK_RAW(sub_8223CDD0) { g_census.Hit(240, ctx); __imp__sub_8223CDD0(ctx, base); }
REX_HOOK_RAW(sub_8223CF10) { g_census.Hit(241, ctx); __imp__sub_8223CF10(ctx, base); }
REX_HOOK_RAW(sub_8223D740) { g_census.Hit(242, ctx); __imp__sub_8223D740(ctx, base); }
REX_HOOK_RAW(sub_8223DA20) { g_census.Hit(243, ctx); __imp__sub_8223DA20(ctx, base); }
REX_HOOK_RAW(sub_8223DA40) { g_census.Hit(244, ctx); __imp__sub_8223DA40(ctx, base); }
REX_HOOK_RAW(sub_8223DA50) { g_census.Hit(245, ctx); __imp__sub_8223DA50(ctx, base); }
REX_HOOK_RAW(sub_8223DA70) { g_census.Hit(246, ctx); __imp__sub_8223DA70(ctx, base); }
REX_HOOK_RAW(sub_8223DA80) { g_census.Hit(247, ctx); __imp__sub_8223DA80(ctx, base); }
REX_HOOK_RAW(sub_8223DAA8) { g_census.Hit(248, ctx); __imp__sub_8223DAA8(ctx, base); }
REX_HOOK_RAW(sub_8223DAB8) { g_census.Hit(249, ctx); __imp__sub_8223DAB8(ctx, base); }
REX_HOOK_RAW(sub_8223DB38) { g_census.Hit(250, ctx); __imp__sub_8223DB38(ctx, base); }
REX_HOOK_RAW(sub_8223DB48) { g_census.Hit(251, ctx); __imp__sub_8223DB48(ctx, base); }
REX_HOOK_RAW(sub_8223DBC8) { g_census.Hit(252, ctx); __imp__sub_8223DBC8(ctx, base); }
REX_HOOK_RAW(sub_8223DBD8) { g_census.Hit(253, ctx); __imp__sub_8223DBD8(ctx, base); }
REX_HOOK_RAW(sub_8223DC58) { g_census.Hit(254, ctx); __imp__sub_8223DC58(ctx, base); }
REX_HOOK_RAW(sub_8223DC68) { g_census.Hit(255, ctx); __imp__sub_8223DC68(ctx, base); }
REX_HOOK_RAW(sub_8223DCE8) { g_census.Hit(256, ctx); __imp__sub_8223DCE8(ctx, base); }
REX_HOOK_RAW(sub_8223DCF8) { g_census.Hit(257, ctx); __imp__sub_8223DCF8(ctx, base); }
REX_HOOK_RAW(sub_8223DD58) { g_census.Hit(258, ctx); __imp__sub_8223DD58(ctx, base); }
REX_HOOK_RAW(sub_8223DD68) { g_census.Hit(259, ctx); __imp__sub_8223DD68(ctx, base); }
REX_HOOK_RAW(sub_8223DDC8) { g_census.Hit(260, ctx); __imp__sub_8223DDC8(ctx, base); }
REX_HOOK_RAW(sub_8223DDD8) { g_census.Hit(261, ctx); __imp__sub_8223DDD8(ctx, base); }
REX_HOOK_RAW(sub_8223DE38) { g_census.Hit(262, ctx); __imp__sub_8223DE38(ctx, base); }
REX_HOOK_RAW(sub_8223DE48) { g_census.Hit(263, ctx); __imp__sub_8223DE48(ctx, base); }
REX_HOOK_RAW(sub_8223DED0) { g_census.Hit(264, ctx); __imp__sub_8223DED0(ctx, base); }
REX_HOOK_RAW(sub_8223DEE0) { g_census.Hit(265, ctx); __imp__sub_8223DEE0(ctx, base); }
REX_HOOK_RAW(sub_8223DF18) { g_census.Hit(266, ctx); __imp__sub_8223DF18(ctx, base); }
REX_HOOK_RAW(sub_8223DF48) { g_census.Hit(267, ctx); __imp__sub_8223DF48(ctx, base); }
REX_HOOK_RAW(sub_8223DF68) { g_census.Hit(268, ctx); __imp__sub_8223DF68(ctx, base); }
REX_HOOK_RAW(sub_8223DF78) { g_census.Hit(269, ctx); __imp__sub_8223DF78(ctx, base); }
REX_HOOK_RAW(sub_8223E018) { g_census.Hit(270, ctx); __imp__sub_8223E018(ctx, base); }
REX_HOOK_RAW(sub_8223E098) { g_census.Hit(271, ctx); __imp__sub_8223E098(ctx, base); }
REX_HOOK_RAW(sub_8223E0C0) { g_census.Hit(272, ctx); __imp__sub_8223E0C0(ctx, base); }
REX_HOOK_RAW(sub_8223E0D0) { g_census.Hit(273, ctx); __imp__sub_8223E0D0(ctx, base); }
REX_HOOK_RAW(sub_8223E110) { g_census.Hit(274, ctx); __imp__sub_8223E110(ctx, base); }
REX_HOOK_RAW(sub_8223E140) { g_census.Hit(275, ctx); __imp__sub_8223E140(ctx, base); }
REX_HOOK_RAW(sub_8223E160) { g_census.Hit(276, ctx); __imp__sub_8223E160(ctx, base); }
REX_HOOK_RAW(sub_8223E170) { g_census.Hit(277, ctx); __imp__sub_8223E170(ctx, base); }
REX_HOOK_RAW(sub_8223E1A8) { g_census.Hit(278, ctx); __imp__sub_8223E1A8(ctx, base); }
REX_HOOK_RAW(sub_8223E1B0) { g_census.Hit(279, ctx); __imp__sub_8223E1B0(ctx, base); }
REX_HOOK_RAW(sub_8223E1D0) { g_census.Hit(280, ctx); __imp__sub_8223E1D0(ctx, base); }
REX_HOOK_RAW(sub_8223E1E0) { g_census.Hit(281, ctx); __imp__sub_8223E1E0(ctx, base); }
REX_HOOK_RAW(sub_8223E208) { g_census.Hit(282, ctx); __imp__sub_8223E208(ctx, base); }
REX_HOOK_RAW(sub_8223E218) { g_census.Hit(283, ctx); __imp__sub_8223E218(ctx, base); }
REX_HOOK_RAW(sub_8223E250) { g_census.Hit(284, ctx); __imp__sub_8223E250(ctx, base); }
REX_HOOK_RAW(sub_8223E258) { g_census.Hit(285, ctx); __imp__sub_8223E258(ctx, base); }
REX_HOOK_RAW(sub_8223E280) { g_census.Hit(286, ctx); __imp__sub_8223E280(ctx, base); }
REX_HOOK_RAW(sub_8223E290) { g_census.Hit(287, ctx); __imp__sub_8223E290(ctx, base); }
REX_HOOK_RAW(sub_8223E2B0) { g_census.Hit(288, ctx); __imp__sub_8223E2B0(ctx, base); }
REX_HOOK_RAW(sub_8223E2C0) { g_census.Hit(289, ctx); __imp__sub_8223E2C0(ctx, base); }
REX_HOOK_RAW(sub_8223E2E8) { g_census.Hit(290, ctx); __imp__sub_8223E2E8(ctx, base); }
REX_HOOK_RAW(sub_8223E2F8) { g_census.Hit(291, ctx); __imp__sub_8223E2F8(ctx, base); }
REX_HOOK_RAW(sub_8223E320) { g_census.Hit(292, ctx); __imp__sub_8223E320(ctx, base); }
REX_HOOK_RAW(sub_8223E330) { g_census.Hit(293, ctx); __imp__sub_8223E330(ctx, base); }
REX_HOOK_RAW(sub_8223E350) { g_census.Hit(294, ctx); __imp__sub_8223E350(ctx, base); }
REX_HOOK_RAW(sub_8223E360) { g_census.Hit(295, ctx); __imp__sub_8223E360(ctx, base); }
REX_HOOK_RAW(sub_8223E380) { g_census.Hit(296, ctx); __imp__sub_8223E380(ctx, base); }
REX_HOOK_RAW(sub_8223E390) { g_census.Hit(297, ctx); __imp__sub_8223E390(ctx, base); }
REX_HOOK_RAW(sub_8223E3B8) { g_census.Hit(298, ctx); __imp__sub_8223E3B8(ctx, base); }
REX_HOOK_RAW(sub_8223E3C8) { g_census.Hit(299, ctx); __imp__sub_8223E3C8(ctx, base); }
REX_HOOK_RAW(sub_8223E3F0) { g_census.Hit(300, ctx); __imp__sub_8223E3F0(ctx, base); }
REX_HOOK_RAW(sub_8223E400) { g_census.Hit(301, ctx); __imp__sub_8223E400(ctx, base); }
REX_HOOK_RAW(sub_8223E420) { g_census.Hit(302, ctx); __imp__sub_8223E420(ctx, base); }
REX_HOOK_RAW(sub_8223E430) { g_census.Hit(303, ctx); __imp__sub_8223E430(ctx, base); }
REX_HOOK_RAW(sub_8223E448) { g_census.Hit(304, ctx); __imp__sub_8223E448(ctx, base); }
REX_HOOK_RAW(sub_8223E450) { g_census.Hit(305, ctx); __imp__sub_8223E450(ctx, base); }
REX_HOOK_RAW(sub_8223E468) { g_census.Hit(306, ctx); __imp__sub_8223E468(ctx, base); }
REX_HOOK_RAW(sub_8223E470) { g_census.Hit(307, ctx); __imp__sub_8223E470(ctx, base); }
REX_HOOK_RAW(sub_8223E488) { g_census.Hit(308, ctx); __imp__sub_8223E488(ctx, base); }
REX_HOOK_RAW(sub_8223E490) { g_census.Hit(309, ctx); __imp__sub_8223E490(ctx, base); }
REX_HOOK_RAW(sub_8223E4A8) { g_census.Hit(310, ctx); __imp__sub_8223E4A8(ctx, base); }
REX_HOOK_RAW(sub_8223E4B0) { g_census.Hit(311, ctx); __imp__sub_8223E4B0(ctx, base); }
REX_HOOK_RAW(sub_8223E4C8) { g_census.Hit(312, ctx); __imp__sub_8223E4C8(ctx, base); }
REX_HOOK_RAW(sub_8223E4D0) { g_census.Hit(313, ctx); __imp__sub_8223E4D0(ctx, base); }
REX_HOOK_RAW(sub_8223E4E8) { g_census.Hit(314, ctx); __imp__sub_8223E4E8(ctx, base); }
REX_HOOK_RAW(sub_8223E4F0) { g_census.Hit(315, ctx); __imp__sub_8223E4F0(ctx, base); }
REX_HOOK_RAW(sub_8223E530) { g_census.Hit(316, ctx); __imp__sub_8223E530(ctx, base); }
REX_HOOK_RAW(sub_8223E540) { g_census.Hit(317, ctx); __imp__sub_8223E540(ctx, base); }
REX_HOOK_RAW(sub_8223E548) { g_census.Hit(318, ctx); __imp__sub_8223E548(ctx, base); }
REX_HOOK_RAW(sub_8223E5F0) { g_census.Hit(319, ctx); __imp__sub_8223E5F0(ctx, base); }
REX_HOOK_RAW(sub_8223E610) { g_census.Hit(320, ctx); __imp__sub_8223E610(ctx, base); }
REX_HOOK_RAW(sub_8223E6A8) { g_census.Hit(321, ctx); __imp__sub_8223E6A8(ctx, base); }
REX_HOOK_RAW(sub_8223E6B8) { g_census.Hit(322, ctx); __imp__sub_8223E6B8(ctx, base); }
REX_HOOK_RAW(sub_8223E6D8) { g_census.Hit(323, ctx); __imp__sub_8223E6D8(ctx, base); }
REX_HOOK_RAW(sub_8223E6E8) { g_census.Hit(324, ctx); __imp__sub_8223E6E8(ctx, base); }
REX_HOOK_RAW(sub_8223E700) { g_census.Hit(325, ctx); __imp__sub_8223E700(ctx, base); }
REX_HOOK_RAW(sub_8223E708) { g_census.Hit(326, ctx); __imp__sub_8223E708(ctx, base); }
REX_HOOK_RAW(sub_8223E740) { g_census.Hit(327, ctx); __imp__sub_8223E740(ctx, base); }
REX_HOOK_RAW(sub_8223E748) { g_census.Hit(328, ctx); __imp__sub_8223E748(ctx, base); }
REX_HOOK_RAW(sub_8223E780) { g_census.Hit(329, ctx); __imp__sub_8223E780(ctx, base); }
REX_HOOK_RAW(sub_8223E788) { g_census.Hit(330, ctx); __imp__sub_8223E788(ctx, base); }
REX_HOOK_RAW(sub_8223E7C0) { g_census.Hit(331, ctx); __imp__sub_8223E7C0(ctx, base); }
REX_HOOK_RAW(sub_8223E7C8) { g_census.Hit(332, ctx); __imp__sub_8223E7C8(ctx, base); }
REX_HOOK_RAW(sub_8223E800) { g_census.Hit(333, ctx); __imp__sub_8223E800(ctx, base); }
REX_HOOK_RAW(sub_8223E808) { g_census.Hit(334, ctx); __imp__sub_8223E808(ctx, base); }
REX_HOOK_RAW(sub_8223E810) { g_census.Hit(335, ctx); __imp__sub_8223E810(ctx, base); }
REX_HOOK_RAW(sub_8223E818) { g_census.Hit(336, ctx); __imp__sub_8223E818(ctx, base); }
REX_HOOK_RAW(sub_8223E868) { g_census.Hit(337, ctx); __imp__sub_8223E868(ctx, base); }
REX_HOOK_RAW(sub_8223E878) { g_census.Hit(338, ctx); __imp__sub_8223E878(ctx, base); }
REX_HOOK_RAW(sub_8223E8C0) { g_census.Hit(339, ctx); __imp__sub_8223E8C0(ctx, base); }
REX_HOOK_RAW(sub_8223E8D0) { g_census.Hit(340, ctx); __imp__sub_8223E8D0(ctx, base); }
REX_HOOK_RAW(sub_8223E918) { g_census.Hit(341, ctx); __imp__sub_8223E918(ctx, base); }
REX_HOOK_RAW(sub_8223E928) { g_census.Hit(342, ctx); __imp__sub_8223E928(ctx, base); }
REX_HOOK_RAW(sub_8223E948) { g_census.Hit(343, ctx); __imp__sub_8223E948(ctx, base); }
REX_HOOK_RAW(sub_8223E970) { g_census.Hit(344, ctx); __imp__sub_8223E970(ctx, base); }
REX_HOOK_RAW(sub_8223E998) { g_census.Hit(345, ctx); __imp__sub_8223E998(ctx, base); }
REX_HOOK_RAW(sub_8223E9C0) { g_census.Hit(346, ctx); __imp__sub_8223E9C0(ctx, base); }
REX_HOOK_RAW(sub_8223E9E8) { g_census.Hit(347, ctx); __imp__sub_8223E9E8(ctx, base); }
REX_HOOK_RAW(sub_8223EA10) { g_census.Hit(348, ctx); __imp__sub_8223EA10(ctx, base); }
REX_HOOK_RAW(sub_8223EA38) { g_census.Hit(349, ctx); __imp__sub_8223EA38(ctx, base); }
REX_HOOK_RAW(sub_8223EA58) { g_census.Hit(350, ctx); __imp__sub_8223EA58(ctx, base); }
REX_HOOK_RAW(sub_8223EA78) { g_census.Hit(351, ctx); __imp__sub_8223EA78(ctx, base); }
REX_HOOK_RAW(sub_8223EAA0) { g_census.Hit(352, ctx); __imp__sub_8223EAA0(ctx, base); }
REX_HOOK_RAW(sub_8223EAC8) { g_census.Hit(353, ctx); __imp__sub_8223EAC8(ctx, base); }
REX_HOOK_RAW(sub_8223EAF0) { g_census.Hit(354, ctx); __imp__sub_8223EAF0(ctx, base); }
REX_HOOK_RAW(sub_8223EB18) { g_census.Hit(355, ctx); __imp__sub_8223EB18(ctx, base); }
REX_HOOK_RAW(sub_8223EB40) { g_census.Hit(356, ctx); __imp__sub_8223EB40(ctx, base); }
REX_HOOK_RAW(sub_8223EB68) { g_census.Hit(357, ctx); __imp__sub_8223EB68(ctx, base); }
REX_HOOK_RAW(sub_8223EB88) { g_census.Hit(358, ctx); __imp__sub_8223EB88(ctx, base); }
REX_HOOK_RAW(sub_8223EB98) { g_census.Hit(359, ctx); __imp__sub_8223EB98(ctx, base); }
REX_HOOK_RAW(sub_8223EBA8) { g_census.Hit(360, ctx); __imp__sub_8223EBA8(ctx, base); }
REX_HOOK_RAW(sub_8223EBB8) { g_census.Hit(361, ctx); __imp__sub_8223EBB8(ctx, base); }
REX_HOOK_RAW(sub_8223EBC8) { g_census.Hit(362, ctx); __imp__sub_8223EBC8(ctx, base); }
REX_HOOK_RAW(sub_8223EBD8) { g_census.Hit(363, ctx); __imp__sub_8223EBD8(ctx, base); }
REX_HOOK_RAW(sub_8223EBE8) { g_census.Hit(364, ctx); __imp__sub_8223EBE8(ctx, base); }
REX_HOOK_RAW(sub_8223EBF8) { g_census.Hit(365, ctx); __imp__sub_8223EBF8(ctx, base); }
REX_HOOK_RAW(sub_8223EC08) { g_census.Hit(366, ctx); __imp__sub_8223EC08(ctx, base); }
REX_HOOK_RAW(sub_8223EC18) { g_census.Hit(367, ctx); __imp__sub_8223EC18(ctx, base); }
REX_HOOK_RAW(sub_8223EC28) { g_census.Hit(368, ctx); __imp__sub_8223EC28(ctx, base); }
REX_HOOK_RAW(sub_8223EC38) { g_census.Hit(369, ctx); __imp__sub_8223EC38(ctx, base); }
REX_HOOK_RAW(sub_8223EC48) { g_census.Hit(370, ctx); __imp__sub_8223EC48(ctx, base); }
REX_HOOK_RAW(sub_8223EC58) { g_census.Hit(371, ctx); __imp__sub_8223EC58(ctx, base); }
REX_HOOK_RAW(sub_8223EC68) { g_census.Hit(372, ctx); __imp__sub_8223EC68(ctx, base); }
REX_HOOK_RAW(sub_8223EC78) { g_census.Hit(373, ctx); __imp__sub_8223EC78(ctx, base); }
REX_HOOK_RAW(sub_8223EC88) { g_census.Hit(374, ctx); __imp__sub_8223EC88(ctx, base); }
REX_HOOK_RAW(sub_8223ECC8) { g_census.Hit(375, ctx); __imp__sub_8223ECC8(ctx, base); }
REX_HOOK_RAW(sub_8223ECD8) { g_census.Hit(376, ctx); __imp__sub_8223ECD8(ctx, base); }
REX_HOOK_RAW(sub_8223ED80) { g_census.Hit(377, ctx); __imp__sub_8223ED80(ctx, base); }
REX_HOOK_RAW(sub_8223ED88) { g_census.Hit(378, ctx); __imp__sub_8223ED88(ctx, base); }
REX_HOOK_RAW(sub_8223EE30) { g_census.Hit(379, ctx); __imp__sub_8223EE30(ctx, base); }
REX_HOOK_RAW(sub_8223EE38) { g_census.Hit(380, ctx); __imp__sub_8223EE38(ctx, base); }
REX_HOOK_RAW(sub_8223EEE0) { g_census.Hit(381, ctx); __imp__sub_8223EEE0(ctx, base); }
REX_HOOK_RAW(sub_8223EEE8) { g_census.Hit(382, ctx); __imp__sub_8223EEE8(ctx, base); }
REX_HOOK_RAW(sub_8223EF90) { g_census.Hit(383, ctx); __imp__sub_8223EF90(ctx, base); }
REX_HOOK_RAW(sub_8223EF98) { g_census.Hit(384, ctx); __imp__sub_8223EF98(ctx, base); }
REX_HOOK_RAW(sub_8223EFC0) { g_census.Hit(385, ctx); __imp__sub_8223EFC0(ctx, base); }
REX_HOOK_RAW(sub_8223EFD0) { g_census.Hit(386, ctx); __imp__sub_8223EFD0(ctx, base); }
REX_HOOK_RAW(sub_8223EFF8) { g_census.Hit(387, ctx); __imp__sub_8223EFF8(ctx, base); }
REX_HOOK_RAW(sub_8223F008) { g_census.Hit(388, ctx); __imp__sub_8223F008(ctx, base); }
REX_HOOK_RAW(sub_8223F030) { g_census.Hit(389, ctx); __imp__sub_8223F030(ctx, base); }
REX_HOOK_RAW(sub_8223F040) { g_census.Hit(390, ctx); __imp__sub_8223F040(ctx, base); }
REX_HOOK_RAW(sub_8223F068) { g_census.Hit(391, ctx); __imp__sub_8223F068(ctx, base); }
REX_HOOK_RAW(sub_8223F078) { g_census.Hit(392, ctx); __imp__sub_8223F078(ctx, base); }
REX_HOOK_RAW(sub_8223F098) { g_census.Hit(393, ctx); __imp__sub_8223F098(ctx, base); }
REX_HOOK_RAW(sub_8223F0A8) { g_census.Hit(394, ctx); __imp__sub_8223F0A8(ctx, base); }
REX_HOOK_RAW(sub_8223F0C8) { g_census.Hit(395, ctx); __imp__sub_8223F0C8(ctx, base); }
REX_HOOK_RAW(sub_8223F0D0) { g_census.Hit(396, ctx); __imp__sub_8223F0D0(ctx, base); }
REX_HOOK_RAW(sub_8223F0F0) { g_census.Hit(397, ctx); __imp__sub_8223F0F0(ctx, base); }
REX_HOOK_RAW(sub_8223F100) { g_census.Hit(398, ctx); __imp__sub_8223F100(ctx, base); }
REX_HOOK_RAW(sub_8223F120) { g_census.Hit(399, ctx); __imp__sub_8223F120(ctx, base); }
REX_HOOK_RAW(sub_8223F138) { g_census.Hit(400, ctx); __imp__sub_8223F138(ctx, base); }
REX_HOOK_RAW(sub_8223F160) { g_census.Hit(401, ctx); __imp__sub_8223F160(ctx, base); }
REX_HOOK_RAW(sub_8223F168) { g_census.Hit(402, ctx); __imp__sub_8223F168(ctx, base); }
REX_HOOK_RAW(sub_8223F190) { g_census.Hit(403, ctx); __imp__sub_8223F190(ctx, base); }
REX_HOOK_RAW(sub_8223F198) { g_census.Hit(404, ctx); __imp__sub_8223F198(ctx, base); }
REX_HOOK_RAW(sub_8223F1B8) { g_census.Hit(405, ctx); __imp__sub_8223F1B8(ctx, base); }
REX_HOOK_RAW(sub_8223F1C0) { g_census.Hit(406, ctx); __imp__sub_8223F1C0(ctx, base); }
REX_HOOK_RAW(sub_8223F1E8) { g_census.Hit(407, ctx); __imp__sub_8223F1E8(ctx, base); }
REX_HOOK_RAW(sub_8223F1F0) { g_census.Hit(408, ctx); __imp__sub_8223F1F0(ctx, base); }
REX_HOOK_RAW(sub_8223F210) { g_census.Hit(409, ctx); __imp__sub_8223F210(ctx, base); }
REX_HOOK_RAW(sub_8223F220) { g_census.Hit(410, ctx); __imp__sub_8223F220(ctx, base); }
REX_HOOK_RAW(sub_8223F278) { g_census.Hit(411, ctx); __imp__sub_8223F278(ctx, base); }
REX_HOOK_RAW(sub_8223F288) { g_census.Hit(412, ctx); __imp__sub_8223F288(ctx, base); }
REX_HOOK_RAW(sub_8223F2A8) { g_census.Hit(413, ctx); __imp__sub_8223F2A8(ctx, base); }
REX_HOOK_RAW(sub_8223F2B8) { g_census.Hit(414, ctx); __imp__sub_8223F2B8(ctx, base); }
REX_HOOK_RAW(sub_8223F2D8) { g_census.Hit(415, ctx); __imp__sub_8223F2D8(ctx, base); }
REX_HOOK_RAW(sub_8223F2E8) { g_census.Hit(416, ctx); __imp__sub_8223F2E8(ctx, base); }
REX_HOOK_RAW(sub_8223F308) { g_census.Hit(417, ctx); __imp__sub_8223F308(ctx, base); }
REX_HOOK_RAW(sub_8223F318) { g_census.Hit(418, ctx); __imp__sub_8223F318(ctx, base); }
REX_HOOK_RAW(sub_8223F330) { g_census.Hit(419, ctx); __imp__sub_8223F330(ctx, base); }
REX_HOOK_RAW(sub_8223F338) { g_census.Hit(420, ctx); __imp__sub_8223F338(ctx, base); }
REX_HOOK_RAW(sub_8223F340) { g_census.Hit(421, ctx); __imp__sub_8223F340(ctx, base); }
REX_HOOK_RAW(sub_8223F348) { g_census.Hit(422, ctx); __imp__sub_8223F348(ctx, base); }
REX_HOOK_RAW(sub_8223F358) { g_census.Hit(423, ctx); __imp__sub_8223F358(ctx, base); }
REX_HOOK_RAW(sub_8223F368) { g_census.Hit(424, ctx); __imp__sub_8223F368(ctx, base); }
REX_HOOK_RAW(sub_8223F428) { g_census.Hit(425, ctx); __imp__sub_8223F428(ctx, base); }
REX_HOOK_RAW(sub_8223F460) { g_census.Hit(426, ctx); __imp__sub_8223F460(ctx, base); }
REX_HOOK_RAW(sub_8223F4E8) { g_census.Hit(427, ctx); __imp__sub_8223F4E8(ctx, base); }
REX_HOOK_RAW(sub_8223F4F8) { g_census.Hit(428, ctx); __imp__sub_8223F4F8(ctx, base); }
REX_HOOK_RAW(sub_8223F5B8) { g_census.Hit(429, ctx); __imp__sub_8223F5B8(ctx, base); }
REX_HOOK_RAW(sub_8223F5F0) { g_census.Hit(430, ctx); __imp__sub_8223F5F0(ctx, base); }
REX_HOOK_RAW(sub_8223F678) { g_census.Hit(431, ctx); __imp__sub_8223F678(ctx, base); }
REX_HOOK_RAW(sub_8223F688) { g_census.Hit(432, ctx); __imp__sub_8223F688(ctx, base); }
REX_HOOK_RAW(sub_8223F6C8) { g_census.Hit(433, ctx); __imp__sub_8223F6C8(ctx, base); }
REX_HOOK_RAW(sub_8223F6E0) { g_census.Hit(434, ctx); __imp__sub_8223F6E0(ctx, base); }
REX_HOOK_RAW(sub_8223F768) { g_census.Hit(435, ctx); __imp__sub_8223F768(ctx, base); }
REX_HOOK_RAW(sub_8223F778) { g_census.Hit(436, ctx); __imp__sub_8223F778(ctx, base); }
REX_HOOK_RAW(sub_8223F7E0) { g_census.Hit(437, ctx); __imp__sub_8223F7E0(ctx, base); }
REX_HOOK_RAW(sub_8223F7F0) { g_census.Hit(438, ctx); __imp__sub_8223F7F0(ctx, base); }
REX_HOOK_RAW(sub_8223F850) { g_census.Hit(439, ctx); __imp__sub_8223F850(ctx, base); }
REX_HOOK_RAW(sub_8223F898) { g_census.Hit(440, ctx); __imp__sub_8223F898(ctx, base); }
REX_HOOK_RAW(sub_8223F8F8) { g_census.Hit(441, ctx); __imp__sub_8223F8F8(ctx, base); }
REX_HOOK_RAW(sub_8223F938) { g_census.Hit(442, ctx); __imp__sub_8223F938(ctx, base); }
REX_HOOK_RAW(sub_8223F9A8) { g_census.Hit(443, ctx); __imp__sub_8223F9A8(ctx, base); }
REX_HOOK_RAW(sub_8223F9B8) { g_census.Hit(444, ctx); __imp__sub_8223F9B8(ctx, base); }
REX_HOOK_RAW(sub_8223FA28) { g_census.Hit(445, ctx); __imp__sub_8223FA28(ctx, base); }
REX_HOOK_RAW(sub_8223FA38) { g_census.Hit(446, ctx); __imp__sub_8223FA38(ctx, base); }
REX_HOOK_RAW(sub_8223FA88) { g_census.Hit(447, ctx); __imp__sub_8223FA88(ctx, base); }
REX_HOOK_RAW(sub_8223FAA8) { g_census.Hit(448, ctx); __imp__sub_8223FAA8(ctx, base); }
REX_HOOK_RAW(sub_8223FAE0) { g_census.Hit(449, ctx); __imp__sub_8223FAE0(ctx, base); }
REX_HOOK_RAW(sub_8223FAF8) { g_census.Hit(450, ctx); __imp__sub_8223FAF8(ctx, base); }
REX_HOOK_RAW(sub_8223FB30) { g_census.Hit(451, ctx); __imp__sub_8223FB30(ctx, base); }
REX_HOOK_RAW(sub_8223FB48) { g_census.Hit(452, ctx); __imp__sub_8223FB48(ctx, base); }
REX_HOOK_RAW(sub_8223FB80) { g_census.Hit(453, ctx); __imp__sub_8223FB80(ctx, base); }
REX_HOOK_RAW(sub_8223FB98) { g_census.Hit(454, ctx); __imp__sub_8223FB98(ctx, base); }
REX_HOOK_RAW(sub_8223FBD8) { g_census.Hit(455, ctx); __imp__sub_8223FBD8(ctx, base); }
REX_HOOK_RAW(sub_8223FBF0) { g_census.Hit(456, ctx); __imp__sub_8223FBF0(ctx, base); }
REX_HOOK_RAW(sub_8223FC30) { g_census.Hit(457, ctx); __imp__sub_8223FC30(ctx, base); }
REX_HOOK_RAW(sub_8223FC48) { g_census.Hit(458, ctx); __imp__sub_8223FC48(ctx, base); }
REX_HOOK_RAW(sub_8223FC88) { g_census.Hit(459, ctx); __imp__sub_8223FC88(ctx, base); }
REX_HOOK_RAW(sub_8223FCA0) { g_census.Hit(460, ctx); __imp__sub_8223FCA0(ctx, base); }
REX_HOOK_RAW(sub_8223FCE0) { g_census.Hit(461, ctx); __imp__sub_8223FCE0(ctx, base); }
REX_HOOK_RAW(sub_8223FCF8) { g_census.Hit(462, ctx); __imp__sub_8223FCF8(ctx, base); }
REX_HOOK_RAW(sub_8223FD40) { g_census.Hit(463, ctx); __imp__sub_8223FD40(ctx, base); }
REX_HOOK_RAW(sub_8223FD58) { g_census.Hit(464, ctx); __imp__sub_8223FD58(ctx, base); }
REX_HOOK_RAW(sub_8223FE58) { g_census.Hit(465, ctx); __imp__sub_8223FE58(ctx, base); }
REX_HOOK_RAW(sub_8223FEA8) { g_census.Hit(466, ctx); __imp__sub_8223FEA8(ctx, base); }
REX_HOOK_RAW(sub_8223FFC8) { g_census.Hit(467, ctx); __imp__sub_8223FFC8(ctx, base); }
REX_HOOK_RAW(sub_82240050) { g_census.Hit(468, ctx); __imp__sub_82240050(ctx, base); }
REX_HOOK_RAW(sub_822400E0) { g_census.Hit(469, ctx); __imp__sub_822400E0(ctx, base); }
REX_HOOK_RAW(sub_82240128) { g_census.Hit(470, ctx); __imp__sub_82240128(ctx, base); }
REX_HOOK_RAW(sub_82240168) { g_census.Hit(471, ctx); __imp__sub_82240168(ctx, base); }
REX_HOOK_RAW(sub_822401A8) { g_census.Hit(472, ctx); __imp__sub_822401A8(ctx, base); }
REX_HOOK_RAW(sub_82240270) { g_census.Hit(473, ctx); __imp__sub_82240270(ctx, base); }
REX_HOOK_RAW(sub_82240338) { g_census.Hit(474, ctx); __imp__sub_82240338(ctx, base); }
REX_HOOK_RAW(sub_82240430) { g_census.Hit(475, ctx); __imp__sub_82240430(ctx, base); }
REX_HOOK_RAW(sub_82240698) { g_census.Hit(476, ctx); __imp__sub_82240698(ctx, base); }
REX_HOOK_RAW(sub_822406A8) { g_census.Hit(477, ctx); __imp__sub_822406A8(ctx, base); }
REX_HOOK_RAW(sub_82240728) { g_census.Hit(478, ctx); __imp__sub_82240728(ctx, base); }
REX_HOOK_RAW(sub_82240748) { g_census.Hit(479, ctx); __imp__sub_82240748(ctx, base); }
REX_HOOK_RAW(sub_822407E0) { g_census.Hit(480, ctx); __imp__sub_822407E0(ctx, base); }
REX_HOOK_RAW(sub_82240B48) { g_census.Hit(481, ctx); __imp__sub_82240B48(ctx, base); }
REX_HOOK_RAW(sub_82240E20) { g_census.Hit(482, ctx); __imp__sub_82240E20(ctx, base); }
REX_HOOK_RAW(sub_82240E28) { g_census.Hit(483, ctx); __imp__sub_82240E28(ctx, base); }
REX_HOOK_RAW(sub_82240E48) { g_census.Hit(484, ctx); __imp__sub_82240E48(ctx, base); }
REX_HOOK_RAW(sub_82240E70) { g_census.Hit(485, ctx); __imp__sub_82240E70(ctx, base); }
REX_HOOK_RAW(sub_822410D8) { g_census.Hit(486, ctx); __imp__sub_822410D8(ctx, base); }
REX_HOOK_RAW(sub_82241318) { g_census.Hit(487, ctx); __imp__sub_82241318(ctx, base); }
REX_HOOK_RAW(sub_822413B8) { g_census.Hit(488, ctx); __imp__sub_822413B8(ctx, base); }
REX_HOOK_RAW(sub_82241410) { g_census.Hit(489, ctx); __imp__sub_82241410(ctx, base); }
REX_HOOK_RAW(sub_822414B8) { g_census.Hit(490, ctx); __imp__sub_822414B8(ctx, base); }
REX_HOOK_RAW(sub_82241530) { g_census.Hit(491, ctx); __imp__sub_82241530(ctx, base); }
REX_HOOK_RAW(sub_822415A0) { g_census.Hit(492, ctx); __imp__sub_822415A0(ctx, base); }
REX_HOOK_RAW(sub_822417C0) { g_census.Hit(493, ctx); __imp__sub_822417C0(ctx, base); }
REX_HOOK_RAW(sub_82241820) { g_census.Hit(494, ctx); __imp__sub_82241820(ctx, base); }
REX_HOOK_RAW(sub_822419E0) { g_census.Hit(495, ctx); __imp__sub_822419E0(ctx, base); }
REX_HOOK_RAW(sub_82241F28) { g_census.Hit(496, ctx); __imp__sub_82241F28(ctx, base); }
REX_HOOK_RAW(sub_822420F8) { g_census.Hit(497, ctx); __imp__sub_822420F8(ctx, base); }
REX_HOOK_RAW(sub_82242210) { g_census.Hit(498, ctx); __imp__sub_82242210(ctx, base); }
REX_HOOK_RAW(sub_82242270) { g_census.Hit(499, ctx); __imp__sub_82242270(ctx, base); }
REX_HOOK_RAW(sub_822423A8) { g_census.Hit(500, ctx); __imp__sub_822423A8(ctx, base); }
REX_HOOK_RAW(sub_822423B8) { g_census.Hit(501, ctx); __imp__sub_822423B8(ctx, base); }
REX_HOOK_RAW(sub_822424A0) { g_census.Hit(502, ctx); __imp__sub_822424A0(ctx, base); }
REX_HOOK_RAW(sub_82242588) { g_census.Hit(503, ctx); __imp__sub_82242588(ctx, base); }
REX_HOOK_RAW(sub_822425E8) { g_census.Hit(504, ctx); __imp__sub_822425E8(ctx, base); }
REX_HOOK_RAW(sub_82242648) { g_census.Hit(505, ctx); __imp__sub_82242648(ctx, base); }
REX_HOOK_RAW(sub_822426A0) { g_census.Hit(506, ctx); __imp__sub_822426A0(ctx, base); }
REX_HOOK_RAW(sub_822426F8) { g_census.Hit(507, ctx); __imp__sub_822426F8(ctx, base); }
REX_HOOK_RAW(sub_822428B8) { g_census.Hit(508, ctx); __imp__sub_822428B8(ctx, base); }
REX_HOOK_RAW(sub_82242900) { g_census.Hit(509, ctx); __imp__sub_82242900(ctx, base); }
REX_HOOK_RAW(sub_822429F8) { g_census.Hit(510, ctx); __imp__sub_822429F8(ctx, base); }
REX_HOOK_RAW(sub_82242BC8) { g_census.Hit(511, ctx); __imp__sub_82242BC8(ctx, base); }
REX_HOOK_RAW(sub_82242C10) { g_census.Hit(512, ctx); __imp__sub_82242C10(ctx, base); }
REX_HOOK_RAW(sub_82242C28) { g_census.Hit(513, ctx); __imp__sub_82242C28(ctx, base); }
REX_HOOK_RAW(sub_82242D18) { g_census.Hit(514, ctx); __imp__sub_82242D18(ctx, base); }
REX_HOOK_RAW(sub_82242DA0) { g_census.Hit(515, ctx); __imp__sub_82242DA0(ctx, base); }
REX_HOOK_RAW(sub_82242E88) { g_census.Hit(516, ctx); __imp__sub_82242E88(ctx, base); }
REX_HOOK_RAW(sub_822430D0) { g_census.Hit(517, ctx); __imp__sub_822430D0(ctx, base); }
REX_HOOK_RAW(sub_82243118) { g_census.Hit(518, ctx); __imp__sub_82243118(ctx, base); }
REX_HOOK_RAW(sub_82243168) { g_census.Hit(519, ctx); __imp__sub_82243168(ctx, base); }
REX_HOOK_RAW(sub_82243218) { g_census.Hit(520, ctx); __imp__sub_82243218(ctx, base); }
REX_HOOK_RAW(sub_822432C8) { g_census.Hit(521, ctx); __imp__sub_822432C8(ctx, base); }
REX_HOOK_RAW(sub_82243468) { g_census.Hit(522, ctx); __imp__sub_82243468(ctx, base); }
REX_HOOK_RAW(sub_822435F0) { g_census.Hit(523, ctx); __imp__sub_822435F0(ctx, base); }
REX_HOOK_RAW(sub_822436D0) { g_census.Hit(524, ctx); __imp__sub_822436D0(ctx, base); }
REX_HOOK_RAW(sub_822437C0) { g_census.Hit(525, ctx); __imp__sub_822437C0(ctx, base); }
REX_HOOK_RAW(sub_82243810) { g_census.Hit(526, ctx); __imp__sub_82243810(ctx, base); }
REX_HOOK_RAW(sub_82243818) { g_census.Hit(527, ctx); __imp__sub_82243818(ctx, base); }
REX_HOOK_RAW(sub_82243880) { g_census.Hit(528, ctx); __imp__sub_82243880(ctx, base); }
REX_HOOK_RAW(sub_82243888) { g_census.Hit(529, ctx); __imp__sub_82243888(ctx, base); }
REX_HOOK_RAW(sub_822439F8) { g_census.Hit(530, ctx); __imp__sub_822439F8(ctx, base); }
REX_HOOK_RAW(sub_82243A08) { g_census.Hit(531, ctx); __imp__sub_82243A08(ctx, base); }
REX_HOOK_RAW(sub_82243A78) { g_census.Hit(532, ctx); __imp__sub_82243A78(ctx, base); }
REX_HOOK_RAW(sub_82243AA8) { g_census.Hit(533, ctx); __imp__sub_82243AA8(ctx, base); }
REX_HOOK_RAW(sub_82243BC0) { g_census.Hit(534, ctx); __imp__sub_82243BC0(ctx, base); }
REX_HOOK_RAW(sub_82244088) { g_census.Hit(535, ctx); __imp__sub_82244088(ctx, base); }
REX_HOOK_RAW(sub_82244110) { g_census.Hit(536, ctx); __imp__sub_82244110(ctx, base); }
REX_HOOK_RAW(sub_82244168) { g_census.Hit(537, ctx); __imp__sub_82244168(ctx, base); }
REX_HOOK_RAW(sub_82244558) { g_census.Hit(538, ctx); __imp__sub_82244558(ctx, base); }
REX_HOOK_RAW(sub_82244850) { g_census.Hit(539, ctx); __imp__sub_82244850(ctx, base); }
REX_HOOK_RAW(sub_82244A10) { g_census.Hit(540, ctx); __imp__sub_82244A10(ctx, base); }
REX_HOOK_RAW(sub_82244B28) { g_census.Hit(541, ctx); __imp__sub_82244B28(ctx, base); }
REX_HOOK_RAW(sub_82244BB8) { g_census.Hit(542, ctx); __imp__sub_82244BB8(ctx, base); }
REX_HOOK_RAW(sub_82244C98) { g_census.Hit(543, ctx); __imp__sub_82244C98(ctx, base); }
REX_HOOK_RAW(sub_82244F40) { g_census.Hit(544, ctx); __imp__sub_82244F40(ctx, base); }
REX_HOOK_RAW(sub_82245068) { g_census.Hit(545, ctx); __imp__sub_82245068(ctx, base); }
REX_HOOK_RAW(sub_822450E0) { g_census.Hit(546, ctx); __imp__sub_822450E0(ctx, base); }
REX_HOOK_RAW(sub_82245140) { g_census.Hit(547, ctx); __imp__sub_82245140(ctx, base); }
REX_HOOK_RAW(sub_822451D8) { g_census.Hit(548, ctx); __imp__sub_822451D8(ctx, base); }
REX_HOOK_RAW(sub_822452D8) { g_census.Hit(549, ctx); __imp__sub_822452D8(ctx, base); }
REX_HOOK_RAW(sub_82245508) { g_census.Hit(550, ctx); __imp__sub_82245508(ctx, base); }
REX_HOOK_RAW(sub_82245590) { g_census.Hit(551, ctx); __imp__sub_82245590(ctx, base); }
REX_HOOK_RAW(sub_822459E8) { g_census.Hit(552, ctx); __imp__sub_822459E8(ctx, base); }
REX_HOOK_RAW(sub_82245B60) { g_census.Hit(553, ctx); __imp__sub_82245B60(ctx, base); }
REX_HOOK_RAW(sub_82245BB8) { g_census.Hit(554, ctx); __imp__sub_82245BB8(ctx, base); }
REX_HOOK_RAW(sub_82245CB0) { g_census.Hit(555, ctx); __imp__sub_82245CB0(ctx, base); }
REX_HOOK_RAW(sub_82245DA8) { g_census.Hit(556, ctx); __imp__sub_82245DA8(ctx, base); }
REX_HOOK_RAW(sub_82246228) { g_census.Hit(557, ctx); __imp__sub_82246228(ctx, base); }
REX_HOOK_RAW(sub_82246508) { g_census.Hit(558, ctx); __imp__sub_82246508(ctx, base); }
REX_HOOK_RAW(sub_82246A50) { g_census.Hit(559, ctx); __imp__sub_82246A50(ctx, base); }
REX_HOOK_RAW(sub_82246C20) { g_census.Hit(560, ctx); __imp__sub_82246C20(ctx, base); }
REX_HOOK_RAW(sub_82246CA0) { g_census.Hit(561, ctx); __imp__sub_82246CA0(ctx, base); }
REX_HOOK_RAW(sub_82246CD0) { g_census.Hit(562, ctx); __imp__sub_82246CD0(ctx, base); }
REX_HOOK_RAW(sub_82246DB8) { g_census.Hit(563, ctx); __imp__sub_82246DB8(ctx, base); }
REX_HOOK_RAW(sub_82246DD0) { g_census.Hit(564, ctx); __imp__sub_82246DD0(ctx, base); }
REX_HOOK_RAW(sub_82246DE8) { g_census.Hit(565, ctx); __imp__sub_82246DE8(ctx, base); }
REX_HOOK_RAW(sub_82246E20) { g_census.Hit(566, ctx); __imp__sub_82246E20(ctx, base); }
REX_HOOK_RAW(sub_82246E78) { g_census.Hit(567, ctx); __imp__sub_82246E78(ctx, base); }
REX_HOOK_RAW(sub_82246EC0) { g_census.Hit(568, ctx); __imp__sub_82246EC0(ctx, base); }
REX_HOOK_RAW(sub_82246ED8) { g_census.Hit(569, ctx); __imp__sub_82246ED8(ctx, base); }
REX_HOOK_RAW(sub_82246EF0) { g_census.Hit(570, ctx); __imp__sub_82246EF0(ctx, base); }
REX_HOOK_RAW(sub_82246F18) { g_census.Hit(571, ctx); __imp__sub_82246F18(ctx, base); }
REX_HOOK_RAW(sub_822471D8) { g_census.Hit(572, ctx); __imp__sub_822471D8(ctx, base); }
REX_HOOK_RAW(sub_82247258) { g_census.Hit(573, ctx); __imp__sub_82247258(ctx, base); }
REX_HOOK_RAW(sub_82247368) { g_census.Hit(574, ctx); __imp__sub_82247368(ctx, base); }
REX_HOOK_RAW(sub_82247A98) { g_census.Hit(575, ctx); __imp__sub_82247A98(ctx, base); }
REX_HOOK_RAW(sub_82247B50) { g_census.Hit(576, ctx); __imp__sub_82247B50(ctx, base); }
REX_HOOK_RAW(sub_82247F20) { g_census.Hit(577, ctx); __imp__sub_82247F20(ctx, base); }
REX_HOOK_RAW(sub_822487A0) { g_census.Hit(578, ctx); __imp__sub_822487A0(ctx, base); }
REX_HOOK_RAW(sub_822487D0) { g_census.Hit(579, ctx); __imp__sub_822487D0(ctx, base); }
REX_HOOK_RAW(sub_82248B70) { g_census.Hit(580, ctx); __imp__sub_82248B70(ctx, base); }
REX_HOOK_RAW(sub_82248C60) { g_census.Hit(581, ctx); __imp__sub_82248C60(ctx, base); }
REX_HOOK_RAW(sub_82248CB0) { g_census.Hit(582, ctx); __imp__sub_82248CB0(ctx, base); }
REX_HOOK_RAW(sub_82248D58) { g_census.Hit(583, ctx); __imp__sub_82248D58(ctx, base); }
REX_HOOK_RAW(sub_82248DC0) { g_census.Hit(584, ctx); __imp__sub_82248DC0(ctx, base); }
REX_HOOK_RAW(sub_82248ED8) { g_census.Hit(585, ctx); __imp__sub_82248ED8(ctx, base); }
REX_HOOK_RAW(sub_82248F34) { g_census.Hit(586, ctx); __imp__sub_82248F34(ctx, base); }
REX_HOOK_RAW(sub_82249268) { g_census.Hit(587, ctx); __imp__sub_82249268(ctx, base); }
REX_HOOK_RAW(sub_82249340) { g_census.Hit(588, ctx); __imp__sub_82249340(ctx, base); }
REX_HOOK_RAW(sub_8224A188) { g_census.Hit(589, ctx); __imp__sub_8224A188(ctx, base); }
REX_HOOK_RAW(sub_8224A1A0) { g_census.Hit(590, ctx); __imp__sub_8224A1A0(ctx, base); }
REX_HOOK_RAW(sub_8224A1C8) { g_census.Hit(591, ctx); __imp__sub_8224A1C8(ctx, base); }
REX_HOOK_RAW(sub_8224A238) { g_census.Hit(592, ctx); __imp__sub_8224A238(ctx, base); }
REX_HOOK_RAW(sub_8224A320) { g_census.Hit(593, ctx); __imp__sub_8224A320(ctx, base); }
REX_HOOK_RAW(sub_8224A3B8) { g_census.Hit(594, ctx); __imp__sub_8224A3B8(ctx, base); }
REX_HOOK_RAW(sub_8224A9C0) { g_census.Hit(595, ctx); __imp__sub_8224A9C0(ctx, base); }
REX_HOOK_RAW(sub_8224A9D8) { g_census.Hit(596, ctx); __imp__sub_8224A9D8(ctx, base); }
REX_HOOK_RAW(sub_8224C010) { g_census.Hit(597, ctx); __imp__sub_8224C010(ctx, base); }
REX_HOOK_RAW(sub_8224C078) { g_census.Hit(598, ctx); __imp__sub_8224C078(ctx, base); }
REX_HOOK_RAW(sub_8224C0E0) { g_census.Hit(599, ctx); __imp__sub_8224C0E0(ctx, base); }
REX_HOOK_RAW(sub_8224C2B0) { g_census.Hit(600, ctx); __imp__sub_8224C2B0(ctx, base); }
REX_HOOK_RAW(sub_8224C328) { g_census.Hit(601, ctx); __imp__sub_8224C328(ctx, base); }
REX_HOOK_RAW(sub_8224C410) { g_census.Hit(602, ctx); __imp__sub_8224C410(ctx, base); }
REX_HOOK_RAW(sub_8224C478) { g_census.Hit(603, ctx); __imp__sub_8224C478(ctx, base); }
REX_HOOK_RAW(sub_8224C578) { g_census.Hit(604, ctx); __imp__sub_8224C578(ctx, base); }
REX_HOOK_RAW(sub_8224C5D8) { g_census.Hit(605, ctx); __imp__sub_8224C5D8(ctx, base); }
REX_HOOK_RAW(sub_8224C778) { g_census.Hit(606, ctx); __imp__sub_8224C778(ctx, base); }
REX_HOOK_RAW(sub_8224C7D8) { g_census.Hit(607, ctx); __imp__sub_8224C7D8(ctx, base); }
REX_HOOK_RAW(sub_8224C860) { g_census.Hit(608, ctx); __imp__sub_8224C860(ctx, base); }
REX_HOOK_RAW(sub_8224C960) { g_census.Hit(609, ctx); __imp__sub_8224C960(ctx, base); }
REX_HOOK_RAW(sub_8224CB00) { g_census.Hit(610, ctx); __imp__sub_8224CB00(ctx, base); }
REX_HOOK_RAW(sub_8224CC48) { g_census.Hit(611, ctx); __imp__sub_8224CC48(ctx, base); }
REX_HOOK_RAW(sub_8224CF80) { g_census.Hit(612, ctx); __imp__sub_8224CF80(ctx, base); }
REX_HOOK_RAW(sub_8224D0A8) { g_census.Hit(613, ctx); __imp__sub_8224D0A8(ctx, base); }
REX_HOOK_RAW(sub_8224D3C0) { g_census.Hit(614, ctx); __imp__sub_8224D3C0(ctx, base); }
REX_HOOK_RAW(sub_8224D4B0) { g_census.Hit(615, ctx); __imp__sub_8224D4B0(ctx, base); }
REX_HOOK_RAW(sub_8224D538) { g_census.Hit(616, ctx); __imp__sub_8224D538(ctx, base); }
REX_HOOK_RAW(sub_8224D588) { g_census.Hit(617, ctx); __imp__sub_8224D588(ctx, base); }
REX_HOOK_RAW(sub_8224D6D8) { g_census.Hit(618, ctx); __imp__sub_8224D6D8(ctx, base); }
REX_HOOK_RAW(sub_8224D768) { g_census.Hit(619, ctx); __imp__sub_8224D768(ctx, base); }
REX_HOOK_RAW(sub_8224DA58) { g_census.Hit(620, ctx); __imp__sub_8224DA58(ctx, base); }
REX_HOOK_RAW(sub_8224DCB8) { g_census.Hit(621, ctx); __imp__sub_8224DCB8(ctx, base); }
REX_HOOK_RAW(sub_8224E220) { g_census.Hit(622, ctx); __imp__sub_8224E220(ctx, base); }
REX_HOOK_RAW(sub_8224E8B0) { g_census.Hit(623, ctx); __imp__sub_8224E8B0(ctx, base); }
REX_HOOK_RAW(sub_8224E9D8) { g_census.Hit(624, ctx); __imp__sub_8224E9D8(ctx, base); }
REX_HOOK_RAW(sub_8224EA98) { g_census.Hit(625, ctx); __imp__sub_8224EA98(ctx, base); }
REX_HOOK_RAW(sub_8224EAF8) { g_census.Hit(626, ctx); __imp__sub_8224EAF8(ctx, base); }
REX_HOOK_RAW(sub_8224EB18) { g_census.Hit(627, ctx); __imp__sub_8224EB18(ctx, base); }
REX_HOOK_RAW(sub_8224EBD8) { g_census.Hit(628, ctx); __imp__sub_8224EBD8(ctx, base); }
REX_HOOK_RAW(sub_8224ECB0) { g_census.Hit(629, ctx); __imp__sub_8224ECB0(ctx, base); }
REX_HOOK_RAW(sub_8224EF70) { g_census.Hit(630, ctx); __imp__sub_8224EF70(ctx, base); }
REX_HOOK_RAW(sub_8224F280) { g_census.Hit(631, ctx); __imp__sub_8224F280(ctx, base); }
REX_HOOK_RAW(sub_8224F320) { g_census.Hit(632, ctx); __imp__sub_8224F320(ctx, base); }
REX_HOOK_RAW(sub_8224F3B0) { g_census.Hit(633, ctx); __imp__sub_8224F3B0(ctx, base); }
REX_HOOK_RAW(sub_8224F3B8) { g_census.Hit(634, ctx); __imp__sub_8224F3B8(ctx, base); }
REX_HOOK_RAW(sub_8224F458) { g_census.Hit(635, ctx); __imp__sub_8224F458(ctx, base); }
REX_HOOK_RAW(sub_8224F518) { g_census.Hit(636, ctx); __imp__sub_8224F518(ctx, base); }
REX_HOOK_RAW(sub_8224F5A8) { g_census.Hit(637, ctx); __imp__sub_8224F5A8(ctx, base); }
REX_HOOK_RAW(sub_8224F600) { g_census.Hit(638, ctx); __imp__sub_8224F600(ctx, base); }
REX_HOOK_RAW(sub_8224F658) { g_census.Hit(639, ctx); __imp__sub_8224F658(ctx, base); }
REX_HOOK_RAW(sub_8224F6B8) { g_census.Hit(640, ctx); __imp__sub_8224F6B8(ctx, base); }
REX_HOOK_RAW(sub_8224F728) { g_census.Hit(641, ctx); __imp__sub_8224F728(ctx, base); }
REX_HOOK_RAW(sub_8224F8E8) { g_census.Hit(642, ctx); __imp__sub_8224F8E8(ctx, base); }
REX_HOOK_RAW(sub_8224F958) { g_census.Hit(643, ctx); __imp__sub_8224F958(ctx, base); }
REX_HOOK_RAW(sub_8224F9C8) { g_census.Hit(644, ctx); __imp__sub_8224F9C8(ctx, base); }
REX_HOOK_RAW(sub_8224FA30) { g_census.Hit(645, ctx); __imp__sub_8224FA30(ctx, base); }
REX_HOOK_RAW(sub_8224FAA0) { g_census.Hit(646, ctx); __imp__sub_8224FAA0(ctx, base); }
REX_HOOK_RAW(sub_8224FD00) { g_census.Hit(647, ctx); __imp__sub_8224FD00(ctx, base); }
REX_HOOK_RAW(sub_8224FD08) { g_census.Hit(648, ctx); __imp__sub_8224FD08(ctx, base); }
REX_HOOK_RAW(sub_8224FD78) { g_census.Hit(649, ctx); __imp__sub_8224FD78(ctx, base); }
REX_HOOK_RAW(sub_8224FDF0) { g_census.Hit(650, ctx); __imp__sub_8224FDF0(ctx, base); }
REX_HOOK_RAW(sub_8224FE78) { g_census.Hit(651, ctx); __imp__sub_8224FE78(ctx, base); }
REX_HOOK_RAW(sub_8224FF40) { g_census.Hit(652, ctx); __imp__sub_8224FF40(ctx, base); }
REX_HOOK_RAW(sub_82250038) { g_census.Hit(653, ctx); __imp__sub_82250038(ctx, base); }
REX_HOOK_RAW(sub_82250198) { g_census.Hit(654, ctx); __imp__sub_82250198(ctx, base); }
REX_HOOK_RAW(sub_82250248) { g_census.Hit(655, ctx); __imp__sub_82250248(ctx, base); }
REX_HOOK_RAW(sub_822502A0) { g_census.Hit(656, ctx); __imp__sub_822502A0(ctx, base); }
REX_HOOK_RAW(sub_822505A8) { g_census.Hit(657, ctx); __imp__sub_822505A8(ctx, base); }
REX_HOOK_RAW(sub_82250690) { g_census.Hit(658, ctx); __imp__sub_82250690(ctx, base); }
REX_HOOK_RAW(sub_822506B8) { g_census.Hit(659, ctx); __imp__sub_822506B8(ctx, base); }
REX_HOOK_RAW(sub_822506E8) { g_census.Hit(660, ctx); __imp__sub_822506E8(ctx, base); }
REX_HOOK_RAW(sub_82250740) { g_census.Hit(661, ctx); __imp__sub_82250740(ctx, base); }
REX_HOOK_RAW(sub_82250778) { g_census.Hit(662, ctx); __imp__sub_82250778(ctx, base); }
REX_HOOK_RAW(sub_82250788) { g_census.Hit(663, ctx); __imp__sub_82250788(ctx, base); }
REX_HOOK_RAW(sub_822507F0) { g_census.Hit(664, ctx); __imp__sub_822507F0(ctx, base); }
REX_HOOK_RAW(sub_82250800) { g_census.Hit(665, ctx); __imp__sub_82250800(ctx, base); }
REX_HOOK_RAW(sub_82250850) { g_census.Hit(666, ctx); __imp__sub_82250850(ctx, base); }
REX_HOOK_RAW(sub_822508D0) { g_census.Hit(667, ctx); __imp__sub_822508D0(ctx, base); }
REX_HOOK_RAW(sub_82250940) { g_census.Hit(668, ctx); __imp__sub_82250940(ctx, base); }
REX_HOOK_RAW(sub_82250998) { g_census.Hit(669, ctx); __imp__sub_82250998(ctx, base); }
REX_HOOK_RAW(sub_82250A68) { g_census.Hit(670, ctx); __imp__sub_82250A68(ctx, base); }
REX_HOOK_RAW(sub_82250AC0) { g_census.Hit(671, ctx); __imp__sub_82250AC0(ctx, base); }
REX_HOOK_RAW(sub_82250B30) { g_census.Hit(672, ctx); __imp__sub_82250B30(ctx, base); }
REX_HOOK_RAW(sub_82250BA0) { g_census.Hit(673, ctx); __imp__sub_82250BA0(ctx, base); }
REX_HOOK_RAW(sub_82250D00) { g_census.Hit(674, ctx); __imp__sub_82250D00(ctx, base); }
REX_HOOK_RAW(sub_82250D58) { g_census.Hit(675, ctx); __imp__sub_82250D58(ctx, base); }
REX_HOOK_RAW(sub_82250E90) { g_census.Hit(676, ctx); __imp__sub_82250E90(ctx, base); }
REX_HOOK_RAW(sub_82250F18) { g_census.Hit(677, ctx); __imp__sub_82250F18(ctx, base); }
REX_HOOK_RAW(sub_82251028) { g_census.Hit(678, ctx); __imp__sub_82251028(ctx, base); }
REX_HOOK_RAW(sub_82251190) { g_census.Hit(679, ctx); __imp__sub_82251190(ctx, base); }
REX_HOOK_RAW(sub_822512E8) { g_census.Hit(680, ctx); __imp__sub_822512E8(ctx, base); }
REX_HOOK_RAW(sub_822513A8) { g_census.Hit(681, ctx); __imp__sub_822513A8(ctx, base); }
REX_HOOK_RAW(sub_822515F8) { g_census.Hit(682, ctx); __imp__sub_822515F8(ctx, base); }
REX_HOOK_RAW(sub_822516E8) { g_census.Hit(683, ctx); __imp__sub_822516E8(ctx, base); }
REX_HOOK_RAW(sub_822517F8) { g_census.Hit(684, ctx); __imp__sub_822517F8(ctx, base); }
REX_HOOK_RAW(sub_82251890) { g_census.Hit(685, ctx); __imp__sub_82251890(ctx, base); }
REX_HOOK_RAW(sub_82251910) { g_census.Hit(686, ctx); __imp__sub_82251910(ctx, base); }
REX_HOOK_RAW(sub_822519A0) { g_census.Hit(687, ctx); __imp__sub_822519A0(ctx, base); }
REX_HOOK_RAW(sub_82251A28) { g_census.Hit(688, ctx); __imp__sub_82251A28(ctx, base); }
REX_HOOK_RAW(sub_82251C08) { g_census.Hit(689, ctx); __imp__sub_82251C08(ctx, base); }
REX_HOOK_RAW(sub_82251C90) { g_census.Hit(690, ctx); __imp__sub_82251C90(ctx, base); }
REX_HOOK_RAW(sub_82251D48) { g_census.Hit(691, ctx); __imp__sub_82251D48(ctx, base); }
REX_HOOK_RAW(sub_82251DB8) { g_census.Hit(692, ctx); __imp__sub_82251DB8(ctx, base); }
REX_HOOK_RAW(sub_82251E28) { g_census.Hit(693, ctx); __imp__sub_82251E28(ctx, base); }
REX_HOOK_RAW(sub_82251EB0) { g_census.Hit(694, ctx); __imp__sub_82251EB0(ctx, base); }
REX_HOOK_RAW(sub_82251F10) { g_census.Hit(695, ctx); __imp__sub_82251F10(ctx, base); }
REX_HOOK_RAW(sub_82251F98) { g_census.Hit(696, ctx); __imp__sub_82251F98(ctx, base); }
REX_HOOK_RAW(sub_82251FF0) { g_census.Hit(697, ctx); __imp__sub_82251FF0(ctx, base); }
REX_HOOK_RAW(sub_82252060) { g_census.Hit(698, ctx); __imp__sub_82252060(ctx, base); }
REX_HOOK_RAW(sub_82252070) { g_census.Hit(699, ctx); __imp__sub_82252070(ctx, base); }
REX_HOOK_RAW(sub_82252080) { g_census.Hit(700, ctx); __imp__sub_82252080(ctx, base); }
REX_HOOK_RAW(sub_82252088) { g_census.Hit(701, ctx); __imp__sub_82252088(ctx, base); }
REX_HOOK_RAW(sub_82252090) { g_census.Hit(702, ctx); __imp__sub_82252090(ctx, base); }
REX_HOOK_RAW(sub_82252108) { g_census.Hit(703, ctx); __imp__sub_82252108(ctx, base); }
REX_HOOK_RAW(sub_82252110) { g_census.Hit(704, ctx); __imp__sub_82252110(ctx, base); }
REX_HOOK_RAW(sub_822521B0) { g_census.Hit(705, ctx); __imp__sub_822521B0(ctx, base); }
REX_HOOK_RAW(sub_822521B8) { g_census.Hit(706, ctx); __imp__sub_822521B8(ctx, base); }
REX_HOOK_RAW(sub_82252288) { g_census.Hit(707, ctx); __imp__sub_82252288(ctx, base); }
REX_HOOK_RAW(sub_82252320) { g_census.Hit(708, ctx); __imp__sub_82252320(ctx, base); }
REX_HOOK_RAW(sub_82252410) { g_census.Hit(709, ctx); __imp__sub_82252410(ctx, base); }
REX_HOOK_RAW(sub_82252418) { g_census.Hit(710, ctx); __imp__sub_82252418(ctx, base); }
REX_HOOK_RAW(sub_82252420) { g_census.Hit(711, ctx); __imp__sub_82252420(ctx, base); }
REX_HOOK_RAW(sub_82252470) { g_census.Hit(712, ctx); __imp__sub_82252470(ctx, base); }
REX_HOOK_RAW(sub_822524F0) { g_census.Hit(713, ctx); __imp__sub_822524F0(ctx, base); }
REX_HOOK_RAW(sub_82252560) { g_census.Hit(714, ctx); __imp__sub_82252560(ctx, base); }
REX_HOOK_RAW(sub_822526A8) { g_census.Hit(715, ctx); __imp__sub_822526A8(ctx, base); }
REX_HOOK_RAW(sub_82252AB8) { g_census.Hit(716, ctx); __imp__sub_82252AB8(ctx, base); }
REX_HOOK_RAW(sub_82252C88) { g_census.Hit(717, ctx); __imp__sub_82252C88(ctx, base); }
REX_HOOK_RAW(sub_82252CE8) { g_census.Hit(718, ctx); __imp__sub_82252CE8(ctx, base); }
REX_HOOK_RAW(sub_82252D50) { g_census.Hit(719, ctx); __imp__sub_82252D50(ctx, base); }
REX_HOOK_RAW(sub_82252F98) { g_census.Hit(720, ctx); __imp__sub_82252F98(ctx, base); }
REX_HOOK_RAW(sub_82252FA0) { g_census.Hit(721, ctx); __imp__sub_82252FA0(ctx, base); }
REX_HOOK_RAW(sub_82252FA8) { g_census.Hit(722, ctx); __imp__sub_82252FA8(ctx, base); }
REX_HOOK_RAW(sub_822530A0) { g_census.Hit(723, ctx); __imp__sub_822530A0(ctx, base); }
REX_HOOK_RAW(sub_822533D8) { g_census.Hit(724, ctx); __imp__sub_822533D8(ctx, base); }
REX_HOOK_RAW(sub_82253468) { g_census.Hit(725, ctx); __imp__sub_82253468(ctx, base); }
REX_HOOK_RAW(sub_82253528) { g_census.Hit(726, ctx); __imp__sub_82253528(ctx, base); }
REX_HOOK_RAW(sub_82253580) { g_census.Hit(727, ctx); __imp__sub_82253580(ctx, base); }
REX_HOOK_RAW(sub_822535D8) { g_census.Hit(728, ctx); __imp__sub_822535D8(ctx, base); }
REX_HOOK_RAW(sub_82253798) { g_census.Hit(729, ctx); __imp__sub_82253798(ctx, base); }
REX_HOOK_RAW(sub_822538A0) { g_census.Hit(730, ctx); __imp__sub_822538A0(ctx, base); }
REX_HOOK_RAW(sub_822538F8) { g_census.Hit(731, ctx); __imp__sub_822538F8(ctx, base); }
REX_HOOK_RAW(sub_82253BB0) { g_census.Hit(732, ctx); __imp__sub_82253BB0(ctx, base); }
REX_HOOK_RAW(sub_82253C40) { g_census.Hit(733, ctx); __imp__sub_82253C40(ctx, base); }
REX_HOOK_RAW(sub_82253E68) { g_census.Hit(734, ctx); __imp__sub_82253E68(ctx, base); }
REX_HOOK_RAW(sub_82253EC8) { g_census.Hit(735, ctx); __imp__sub_82253EC8(ctx, base); }
REX_HOOK_RAW(sub_822540A0) { g_census.Hit(736, ctx); __imp__sub_822540A0(ctx, base); }
REX_HOOK_RAW(sub_82254130) { g_census.Hit(737, ctx); __imp__sub_82254130(ctx, base); }
REX_HOOK_RAW(sub_82254278) { g_census.Hit(738, ctx); __imp__sub_82254278(ctx, base); }
REX_HOOK_RAW(sub_82254300) { g_census.Hit(739, ctx); __imp__sub_82254300(ctx, base); }
REX_HOOK_RAW(sub_822543A8) { g_census.Hit(740, ctx); __imp__sub_822543A8(ctx, base); }
REX_HOOK_RAW(sub_822543B0) { g_census.Hit(741, ctx); __imp__sub_822543B0(ctx, base); }
REX_HOOK_RAW(sub_82254478) { g_census.Hit(742, ctx); __imp__sub_82254478(ctx, base); }
REX_HOOK_RAW(sub_822544D0) { g_census.Hit(743, ctx); __imp__sub_822544D0(ctx, base); }
REX_HOOK_RAW(sub_822545A8) { g_census.Hit(744, ctx); __imp__sub_822545A8(ctx, base); }
REX_HOOK_RAW(sub_82254758) { g_census.Hit(745, ctx); __imp__sub_82254758(ctx, base); }
REX_HOOK_RAW(sub_822547E8) { g_census.Hit(746, ctx); __imp__sub_822547E8(ctx, base); }
REX_HOOK_RAW(sub_82254878) { g_census.Hit(747, ctx); __imp__sub_82254878(ctx, base); }
REX_HOOK_RAW(sub_82254930) { g_census.Hit(748, ctx); __imp__sub_82254930(ctx, base); }
REX_HOOK_RAW(sub_82254940) { g_census.Hit(749, ctx); __imp__sub_82254940(ctx, base); }
REX_HOOK_RAW(sub_82254A30) { g_census.Hit(750, ctx); __imp__sub_82254A30(ctx, base); }
REX_HOOK_RAW(sub_82254A88) { g_census.Hit(751, ctx); __imp__sub_82254A88(ctx, base); }
REX_HOOK_RAW(sub_82254A90) { g_census.Hit(752, ctx); __imp__sub_82254A90(ctx, base); }
REX_HOOK_RAW(sub_82254A98) { g_census.Hit(753, ctx); __imp__sub_82254A98(ctx, base); }
REX_HOOK_RAW(sub_82254AF0) { g_census.Hit(754, ctx); __imp__sub_82254AF0(ctx, base); }
REX_HOOK_RAW(sub_82254B78) { g_census.Hit(755, ctx); __imp__sub_82254B78(ctx, base); }
REX_HOOK_RAW(sub_82254C08) { g_census.Hit(756, ctx); __imp__sub_82254C08(ctx, base); }
REX_HOOK_RAW(sub_82254CD8) { g_census.Hit(757, ctx); __imp__sub_82254CD8(ctx, base); }
REX_HOOK_RAW(sub_82254E70) { g_census.Hit(758, ctx); __imp__sub_82254E70(ctx, base); }
REX_HOOK_RAW(sub_82254F10) { g_census.Hit(759, ctx); __imp__sub_82254F10(ctx, base); }
REX_HOOK_RAW(sub_82254F18) { g_census.Hit(760, ctx); __imp__sub_82254F18(ctx, base); }
REX_HOOK_RAW(sub_822550A8) { g_census.Hit(761, ctx); __imp__sub_822550A8(ctx, base); }
REX_HOOK_RAW(sub_82255118) { g_census.Hit(762, ctx); __imp__sub_82255118(ctx, base); }
REX_HOOK_RAW(sub_82255178) { g_census.Hit(763, ctx); __imp__sub_82255178(ctx, base); }
REX_HOOK_RAW(sub_822551D0) { g_census.Hit(764, ctx); __imp__sub_822551D0(ctx, base); }
REX_HOOK_RAW(sub_82255350) { g_census.Hit(765, ctx); __imp__sub_82255350(ctx, base); }
REX_HOOK_RAW(sub_82255380) { g_census.Hit(766, ctx); __imp__sub_82255380(ctx, base); }
REX_HOOK_RAW(sub_822554C8) { g_census.Hit(767, ctx); __imp__sub_822554C8(ctx, base); }
REX_HOOK_RAW(sub_82255740) { g_census.Hit(768, ctx); __imp__sub_82255740(ctx, base); }
REX_HOOK_RAW(sub_82255948) { g_census.Hit(769, ctx); __imp__sub_82255948(ctx, base); }
REX_HOOK_RAW(sub_82255AF8) { g_census.Hit(770, ctx); __imp__sub_82255AF8(ctx, base); }
REX_HOOK_RAW(sub_82255DF0) { g_census.Hit(771, ctx); __imp__sub_82255DF0(ctx, base); }
REX_HOOK_RAW(sub_82255E68) { g_census.Hit(772, ctx); __imp__sub_82255E68(ctx, base); }
REX_HOOK_RAW(sub_82255EE0) { g_census.Hit(773, ctx); __imp__sub_82255EE0(ctx, base); }
REX_HOOK_RAW(sub_82255EF8) { g_census.Hit(774, ctx); __imp__sub_82255EF8(ctx, base); }
REX_HOOK_RAW(sub_82255F98) { g_census.Hit(775, ctx); __imp__sub_82255F98(ctx, base); }
REX_HOOK_RAW(sub_822560B8) { g_census.Hit(776, ctx); __imp__sub_822560B8(ctx, base); }
REX_HOOK_RAW(sub_822560C0) { g_census.Hit(777, ctx); __imp__sub_822560C0(ctx, base); }
REX_HOOK_RAW(sub_822560C8) { g_census.Hit(778, ctx); __imp__sub_822560C8(ctx, base); }
REX_HOOK_RAW(sub_822561A8) { g_census.Hit(779, ctx); __imp__sub_822561A8(ctx, base); }
REX_HOOK_RAW(sub_82256208) { g_census.Hit(780, ctx); __imp__sub_82256208(ctx, base); }
REX_HOOK_RAW(sub_82256498) { g_census.Hit(781, ctx); __imp__sub_82256498(ctx, base); }
REX_HOOK_RAW(sub_822565E8) { g_census.Hit(782, ctx); __imp__sub_822565E8(ctx, base); }
REX_HOOK_RAW(sub_82256670) { g_census.Hit(783, ctx); __imp__sub_82256670(ctx, base); }
REX_HOOK_RAW(sub_822566F0) { g_census.Hit(784, ctx); __imp__sub_822566F0(ctx, base); }
REX_HOOK_RAW(sub_82256780) { g_census.Hit(785, ctx); __imp__sub_82256780(ctx, base); }
REX_HOOK_RAW(sub_822567F0) { g_census.Hit(786, ctx); __imp__sub_822567F0(ctx, base); }
REX_HOOK_RAW(sub_82256898) { g_census.Hit(787, ctx); __imp__sub_82256898(ctx, base); }
REX_HOOK_RAW(sub_822568F0) { g_census.Hit(788, ctx); __imp__sub_822568F0(ctx, base); }
REX_HOOK_RAW(sub_82256A90) { g_census.Hit(789, ctx); __imp__sub_82256A90(ctx, base); }
REX_HOOK_RAW(sub_82256B38) { g_census.Hit(790, ctx); __imp__sub_82256B38(ctx, base); }
REX_HOOK_RAW(sub_82256BC8) { g_census.Hit(791, ctx); __imp__sub_82256BC8(ctx, base); }
REX_HOOK_RAW(sub_82256CD0) { g_census.Hit(792, ctx); __imp__sub_82256CD0(ctx, base); }
REX_HOOK_RAW(sub_82256CF8) { g_census.Hit(793, ctx); __imp__sub_82256CF8(ctx, base); }
REX_HOOK_RAW(sub_82256D90) { g_census.Hit(794, ctx); __imp__sub_82256D90(ctx, base); }
REX_HOOK_RAW(sub_82256E00) { g_census.Hit(795, ctx); __imp__sub_82256E00(ctx, base); }
REX_HOOK_RAW(sub_82256E70) { g_census.Hit(796, ctx); __imp__sub_82256E70(ctx, base); }
REX_HOOK_RAW(sub_82256F20) { g_census.Hit(797, ctx); __imp__sub_82256F20(ctx, base); }
REX_HOOK_RAW(sub_82256F28) { g_census.Hit(798, ctx); __imp__sub_82256F28(ctx, base); }
REX_HOOK_RAW(sub_82256F30) { g_census.Hit(799, ctx); __imp__sub_82256F30(ctx, base); }
REX_HOOK_RAW(sub_82256F80) { g_census.Hit(800, ctx); __imp__sub_82256F80(ctx, base); }
REX_HOOK_RAW(sub_82256FC0) { g_census.Hit(801, ctx); __imp__sub_82256FC0(ctx, base); }
REX_HOOK_RAW(sub_82257258) { g_census.Hit(802, ctx); __imp__sub_82257258(ctx, base); }
REX_HOOK_RAW(sub_82257500) { g_census.Hit(803, ctx); __imp__sub_82257500(ctx, base); }
REX_HOOK_RAW(sub_82257618) { g_census.Hit(804, ctx); __imp__sub_82257618(ctx, base); }
REX_HOOK_RAW(sub_82257678) { g_census.Hit(805, ctx); __imp__sub_82257678(ctx, base); }
REX_HOOK_RAW(sub_82257860) { g_census.Hit(806, ctx); __imp__sub_82257860(ctx, base); }
REX_HOOK_RAW(sub_822578B8) { g_census.Hit(807, ctx); __imp__sub_822578B8(ctx, base); }
REX_HOOK_RAW(sub_82257938) { g_census.Hit(808, ctx); __imp__sub_82257938(ctx, base); }
REX_HOOK_RAW(sub_82257A10) { g_census.Hit(809, ctx); __imp__sub_82257A10(ctx, base); }
REX_HOOK_RAW(sub_82257A28) { g_census.Hit(810, ctx); __imp__sub_82257A28(ctx, base); }
REX_HOOK_RAW(sub_82257A40) { g_census.Hit(811, ctx); __imp__sub_82257A40(ctx, base); }
REX_HOOK_RAW(sub_82257A50) { g_census.Hit(812, ctx); __imp__sub_82257A50(ctx, base); }
REX_HOOK_RAW(sub_82257A58) { g_census.Hit(813, ctx); __imp__sub_82257A58(ctx, base); }
REX_HOOK_RAW(sub_82257A60) { g_census.Hit(814, ctx); __imp__sub_82257A60(ctx, base); }
REX_HOOK_RAW(sub_82257AF0) { g_census.Hit(815, ctx); __imp__sub_82257AF0(ctx, base); }
REX_HOOK_RAW(sub_82257B70) { g_census.Hit(816, ctx); __imp__sub_82257B70(ctx, base); }
REX_HOOK_RAW(sub_82257BC8) { g_census.Hit(817, ctx); __imp__sub_82257BC8(ctx, base); }
REX_HOOK_RAW(sub_82257BD0) { g_census.Hit(818, ctx); __imp__sub_82257BD0(ctx, base); }
REX_HOOK_RAW(sub_82257BD8) { g_census.Hit(819, ctx); __imp__sub_82257BD8(ctx, base); }
REX_HOOK_RAW(sub_82257CF0) { g_census.Hit(820, ctx); __imp__sub_82257CF0(ctx, base); }
REX_HOOK_RAW(sub_82257CF8) { g_census.Hit(821, ctx); __imp__sub_82257CF8(ctx, base); }
REX_HOOK_RAW(sub_82257D00) { g_census.Hit(822, ctx); __imp__sub_82257D00(ctx, base); }
REX_HOOK_RAW(sub_82257D28) { g_census.Hit(823, ctx); __imp__sub_82257D28(ctx, base); }
REX_HOOK_RAW(sub_82257DA0) { g_census.Hit(824, ctx); __imp__sub_82257DA0(ctx, base); }
REX_HOOK_RAW(sub_82257EF8) { g_census.Hit(825, ctx); __imp__sub_82257EF8(ctx, base); }
REX_HOOK_RAW(sub_825AA0E0) { g_census.Hit(826, ctx); __imp__sub_825AA0E0(ctx, base); }
REX_HOOK_RAW(sub_825AA2E0) { g_census.Hit(827, ctx); __imp__sub_825AA2E0(ctx, base); }
REX_HOOK_RAW(sub_825AA360) { g_census.Hit(828, ctx); __imp__sub_825AA360(ctx, base); }
REX_HOOK_RAW(sub_825AA3D0) { g_census.Hit(829, ctx); __imp__sub_825AA3D0(ctx, base); }
REX_HOOK_RAW(sub_825AA4D0) { g_census.Hit(830, ctx); __imp__sub_825AA4D0(ctx, base); }
REX_HOOK_RAW(sub_825AA8D0) { g_census.Hit(831, ctx); __imp__sub_825AA8D0(ctx, base); }
REX_HOOK_RAW(sub_825AA988) { g_census.Hit(832, ctx); __imp__sub_825AA988(ctx, base); }
REX_HOOK_RAW(sub_825AA9F0) { g_census.Hit(833, ctx); __imp__sub_825AA9F0(ctx, base); }
REX_HOOK_RAW(sub_825AAA00) { g_census.Hit(834, ctx); __imp__sub_825AAA00(ctx, base); }
REX_HOOK_RAW(sub_825AAC58) { g_census.Hit(835, ctx); __imp__sub_825AAC58(ctx, base); }
REX_HOOK_RAW(sub_825AAE90) { g_census.Hit(836, ctx); __imp__sub_825AAE90(ctx, base); }
REX_HOOK_RAW(sub_825AAE98) { g_census.Hit(837, ctx); __imp__sub_825AAE98(ctx, base); }
REX_HOOK_RAW(sub_825AAF28) { g_census.Hit(838, ctx); __imp__sub_825AAF28(ctx, base); }
REX_HOOK_RAW(sub_825AB3C8) { g_census.Hit(839, ctx); __imp__sub_825AB3C8(ctx, base); }
REX_HOOK_RAW(sub_825AB3E0) { g_census.Hit(840, ctx); __imp__sub_825AB3E0(ctx, base); }
REX_HOOK_RAW(sub_825AB3E8) { g_census.Hit(841, ctx); __imp__sub_825AB3E8(ctx, base); }
REX_HOOK_RAW(sub_825AB7D0) { g_census.Hit(842, ctx); __imp__sub_825AB7D0(ctx, base); }
REX_HOOK_RAW(sub_825ABC28) { g_census.Hit(843, ctx); __imp__sub_825ABC28(ctx, base); }
REX_HOOK_RAW(sub_825ABC70) { g_census.Hit(844, ctx); __imp__sub_825ABC70(ctx, base); }
REX_HOOK_RAW(sub_825ABCA0) { g_census.Hit(845, ctx); __imp__sub_825ABCA0(ctx, base); }
REX_HOOK_RAW(sub_825ABD00) { g_census.Hit(846, ctx); __imp__sub_825ABD00(ctx, base); }
