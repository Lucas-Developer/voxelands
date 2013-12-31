/*
Minetest-c55
Copyright (C) 2010-2011 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef CONTENT_MAPNODE_HEADER
#define CONTENT_MAPNODE_HEADER

#include "mapnode.h"

void content_mapnode_init();

extern content_t trans_table_19[21][2];

MapNode mapnode_translate_from_internal(MapNode n_from, u8 version);
MapNode mapnode_translate_to_internal(MapNode n_from, u8 version);

/*
	Node content type IDs
	Ranges:
*/

// 0x000...0x07f (0...127): param2 is fully usable
// 126 and 127 are reserved.
// Use these sparingly, only when the extra space in param2 might be needed.
// Add a space when there is unused space between numbers.
#define CONTENT_STONE 0
// FREE 1
#define CONTENT_WATER 2
#define CONTENT_TORCH 3
#define CONTENT_GLASSLIGHT 4
#define CONTENT_CHAIR 5
// FREE 6-7
#define CONTENT_TABLE 8
#define CONTENT_WATERSOURCE 9
// FREE 10-13
#define CONTENT_SIGN_WALL 14
#define CONTENT_CHEST 15
#define CONTENT_FURNACE 16
#define CONTENT_LOCKABLE_CHEST 17
#define CONTENT_SIGN 18
#define CONTENT_SIGN_UD 19
// FREE 20
#define CONTENT_FENCE 21
// FREE 22-29
#define CONTENT_RAIL 30
#define CONTENT_LADDER 31
#define CONTENT_LAVA 32
#define CONTENT_LAVASOURCE 33
// FREE 34-125

// 0x800...0xfff (2048...4095): higher 4 bytes of param2 are not usable
#define CONTENT_GRASS 0x800
#define CONTENT_TREE 0x801
#define CONTENT_LEAVES 0x802
#define CONTENT_FARM_DIRT 0x803
#define CONTENT_MESE 0x804
#define CONTENT_MUD 0x805
#define CONTENT_COTTON 0x806
#define CONTENT_BORDERSTONE 0x807
#define CONTENT_WOOD 0x808
#define CONTENT_SAND 0x809
#define CONTENT_ROUGHSTONE 0x80a
#define CONTENT_STEEL 0x80b
#define CONTENT_GLASS 0x80c
#define CONTENT_MOSSYCOBBLE 0x80d
#define CONTENT_GRAVEL 0x80e
#define CONTENT_SANDSTONE 0x80f
#define CONTENT_CACTUS 0x810
#define CONTENT_BRICK 0x811
#define CONTENT_CLAY 0x812
#define CONTENT_PAPYRUS 0x813
#define CONTENT_BOOKSHELF 0x814
#define CONTENT_JUNGLETREE 0x815
#define CONTENT_JUNGLEGRASS 0x816
#define CONTENT_NC 0x817
#define CONTENT_NC_RB 0x818
#define CONTENT_APPLE 0x819
#define CONTENT_JUNGLEWOOD 0x81a
#define CONTENT_STONEBRICK 0x81b
#define CONTENT_STONEBLOCK 0x81c
#define CONTENT_SAPLING 0x820

// old slab/stair
// old door/hatch/gate
// FREE 821-87F

// plants
#define CONTENT_WILDGRASS_SHORT 0x880
#define CONTENT_WILDGRASS_LONG 0x881
#define CONTENT_DEADGRASS 0x882
#define CONTENT_FLOWER_STEM 0x883
#define CONTENT_FLOWER_ROSE 0x884
#define CONTENT_FLOWER_DAFFODIL 0x885
#define CONTENT_FLOWER_TULIP 0x886
#define CONTENT_FLOWER_POT_RAW 0x887
#define CONTENT_FLOWER_POT 0x888
#define CONTENT_TRIMMED_LEAVES 0x889
#define CONTENT_PLANTS_MIN 0x880
#define CONTENT_PLANTS_MAX 0x886

// coloured cotton
#define CONTENT_COTTON_BLUE 0x890
#define CONTENT_COTTON_GREEN 0x891
#define CONTENT_COTTON_ORANGE 0x892
#define CONTENT_COTTON_PURPLE 0x893
#define CONTENT_COTTON_RED 0x894
#define CONTENT_COTTON_YELLOW 0x895
#define CONTENT_COTTON_BLACK 0x896
// walls
#define CONTENT_ROUGHSTONE_WALL 0x89A
#define CONTENT_MOSSYCOBBLE_WALL 0x89B
#define CONTENT_SANDSTONE_WALL 0x89C
#define CONTENT_STONE_WALL 0x89D
#define CONTENT_COBBLE_WALL 0x89E
// sponge
#define CONTENT_SPONGE 0x8A0
#define CONTENT_SPONGE_FULL 0x8A1
// more
#define CONTENT_HAY 0x8A2
#define CONTENT_SANDSTONE_BRICK 0x8A3
#define CONTENT_SANDSTONE_BLOCK 0x8A4
#define CONTENT_TERRACOTTA 0x8A5
#define CONTENT_TERRACOTTA_BRICK 0x8A6
#define CONTENT_TERRACOTTA_BLOCK 0x8A7
#define CONTENT_TERRACOTTA_TILE 0x8A8
#define CONTENT_CLAY_BLUE 0x8A9
#define CONTENT_CLAY_GREEN 0x8AA
#define CONTENT_CLAY_ORANGE 0x8AB
#define CONTENT_CLAY_PURPLE 0x8AC
#define CONTENT_CLAY_RED 0x8AD
#define CONTENT_CLAY_YELLOW 0x8AE
#define CONTENT_CLAY_BLACK 0x8AF
#define CONTENT_GLASS_BLUE 0x8B0
#define CONTENT_GLASS_GREEN 0x8B1
#define CONTENT_GLASS_ORANGE 0x8B2
#define CONTENT_GLASS_PURPLE 0x8B3
#define CONTENT_GLASS_RED 0x8B4
#define CONTENT_GLASS_YELLOW 0x8B5
#define CONTENT_GLASS_BLACK 0x8B6
#define CONTENT_CARPET 0x8B7
#define CONTENT_CARPET_BLUE 0x8B8
#define CONTENT_CARPET_GREEN 0x8B9
#define CONTENT_CARPET_ORANGE 0x8BA
#define CONTENT_CARPET_PURPLE 0x8BB
#define CONTENT_CARPET_RED 0x8BC
#define CONTENT_CARPET_YELLOW 0x8BD
#define CONTENT_CARPET_BLACK 0x8BE
#define CONTENT_COAL 0x8BF
#define CONTENT_CHARCOAL 0x8C0
#define CONTENT_TIN 0x8C1
#define CONTENT_COPPER 0x8C2
#define CONTENT_SILVER 0x8C3
#define CONTENT_GOLD 0x8C4
#define CONTENT_QUARTZ 0x8C5
// fire and tnt
#define CONTENT_FIRE 0x8C6
#define CONTENT_FIRE_SHORTTERM 0x8C7
#define CONTENT_TNT 0x8C8
#define CONTENT_FLASH 0x8C9
#define CONTENT_MESE_DIGGING 0x8CA
#define CONTENT_COBBLE 0x8CB
#define CONTENT_ROUGHSTONEBRICK 0x8CC
#define CONTENT_ROUGHSTONEBLOCK 0x8CD
#define CONTENT_STEAM 0x8CE
#define CONTENT_INCINERATOR 0x8CF
#define CONTENT_ASH 0x8D0
#define CONTENT_GRASS_FOOTSTEPS 0x8D1
// FREE 8D2-8FF
// glass pane
#define CONTENT_GLASS_PANE 0x900
#define CONTENT_GLASS_PANE_BLUE 0x901
#define CONTENT_GLASS_PANE_GREEN 0x902
#define CONTENT_GLASS_PANE_ORANGE 0x903
#define CONTENT_GLASS_PANE_PURPLE 0x904
#define CONTENT_GLASS_PANE_RED 0x905
#define CONTENT_GLASS_PANE_YELLOW 0x906
#define CONTENT_GLASS_PANE_BLACK 0x907
// more more
#define CONTENT_JUNGLELEAVES 0x910
#define CONTENT_JUNGLESAPLING 0x911
#define CONTENT_APPLE_LEAVES 0x912
#define CONTENT_APPLE_SAPLING 0x913
#define CONTENT_APPLE_TREE 0x914
#define CONTENT_TRIMMED_APPLE_LEAVES 0x915
#define CONTENT_TRIMMED_JUNGLE_LEAVES 0x916
#define CONTENT_TRIMMED_CONIFER_LEAVES 0x917
#define CONTENT_APPLE_BLOSSOM 0x918
// FREE 919-9FE
//bwahaha!
#define CONTENT_CRAFT_GUIDE 0x9FF

// slabs
#define CONTENT_ROUGHSTONE_SLAB 0xA00
#define CONTENT_MOSSYCOBBLE_SLAB 0xA01
#define CONTENT_STONE_SLAB 0xA02
#define CONTENT_WOOD_SLAB 0xA03
#define CONTENT_JUNGLE_SLAB 0xA04
#define CONTENT_BRICK_SLAB 0xA05
#define CONTENT_SANDSTONE_SLAB 0xA06
#define CONTENT_COBBLE_SLAB 0xA07
#define CONTENT_GLASS_SLAB 0xA08
#define CONTENT_GLASS_BLUE_SLAB 0xA09
#define CONTENT_GLASS_GREEN_SLAB 0xA0A
#define CONTENT_GLASS_ORANGE_SLAB 0xA0B
#define CONTENT_GLASS_PURPLE_SLAB 0xA0C
#define CONTENT_GLASS_RED_SLAB 0xA0D
#define CONTENT_GLASS_YELLOW_SLAB 0xA0E
#define CONTENT_GLASS_BLACK_SLAB 0xA0F
// stairs
#define CONTENT_ROUGHSTONE_STAIR 0xA20
#define CONTENT_MOSSYCOBBLE_STAIR 0xA21
#define CONTENT_STONE_STAIR 0xA22
#define CONTENT_WOOD_STAIR 0xA23
#define CONTENT_JUNGLE_STAIR 0xA24
#define CONTENT_BRICK_STAIR 0xA25
#define CONTENT_SANDSTONE_STAIR 0xA26
#define CONTENT_COBBLE_STAIR 0xA27
// upside down slabs
#define CONTENT_ROUGHSTONE_SLAB_UD 0xA40
#define CONTENT_MOSSYCOBBLE_SLAB_UD 0xA41
#define CONTENT_STONE_SLAB_UD 0xA42
#define CONTENT_WOOD_SLAB_UD 0xA43
#define CONTENT_JUNGLE_SLAB_UD 0xA44
#define CONTENT_BRICK_SLAB_UD 0xA45
#define CONTENT_SANDSTONE_SLAB_UD 0xA46
#define CONTENT_COBBLE_SLAB_UD 0xA47
#define CONTENT_GLASS_SLAB_UD 0xA48
#define CONTENT_GLASS_BLUE_SLAB_UD 0xA49
#define CONTENT_GLASS_GREEN_SLAB_UD 0xA4A
#define CONTENT_GLASS_ORANGE_SLAB_UD 0xA4B
#define CONTENT_GLASS_PURPLE_SLAB_UD 0xA4C
#define CONTENT_GLASS_RED_SLAB_UD 0xA4D
#define CONTENT_GLASS_YELLOW_SLAB_UD 0xA4E
#define CONTENT_GLASS_BLACK_SLAB_UD 0xA4F
// upside down stairs
#define CONTENT_ROUGHSTONE_STAIR_UD 0xA60
#define CONTENT_MOSSYCOBBLE_STAIR_UD 0xA61
#define CONTENT_STONE_STAIR_UD 0xA62
#define CONTENT_WOOD_STAIR_UD 0xA63
#define CONTENT_JUNGLE_STAIR_UD 0xA64
#define CONTENT_BRICK_STAIR_UD 0xA65
#define CONTENT_SANDSTONE_STAIR_UD 0xA66
#define CONTENT_COBBLE_STAIR_UD 0xA67
// slab/stair masks
#define CONTENT_SLAB_STAIR_MIN 0xA00
#define CONTENT_SLAB_STAIR_MAX 0xA6F
#define CONTENT_SLAB_STAIR_FLIP 0x040
#define CONTENT_SLAB_STAIR_UD_MIN 0xA40
#define CONTENT_SLAB_STAIR_UD_MAX 0xA6F
// FREE A70-AFF
// doors
#define CONTENT_WOOD_DOOR_LB 0xB00
#define CONTENT_WOOD_DOOR_LT 0xB01
#define CONTENT_STEEL_DOOR_LB 0xB02
#define CONTENT_STEEL_DOOR_LT 0xB03
#define CONTENT_GLASS_DOOR_LB 0xB04
#define CONTENT_GLASS_DOOR_LT 0xB05
// windowed doors
#define CONTENT_WOOD_W_DOOR_LB 0xB10
#define CONTENT_WOOD_W_DOOR_LT 0xB11
#define CONTENT_STEEL_W_DOOR_LB 0xB12
#define CONTENT_STEEL_W_DOOR_LT 0xB13
// right doors
#define CONTENT_WOOD_DOOR_RB 0xB20
#define CONTENT_WOOD_DOOR_RT 0xB21
#define CONTENT_STEEL_DOOR_RB 0xB22
#define CONTENT_STEEL_DOOR_RT 0xB23
#define CONTENT_GLASS_DOOR_RB 0xB24
#define CONTENT_GLASS_DOOR_RT 0xB25
// right windowed doors
#define CONTENT_WOOD_W_DOOR_RB 0xB30
#define CONTENT_WOOD_W_DOOR_RT 0xB31
#define CONTENT_STEEL_W_DOOR_RB 0xB32
#define CONTENT_STEEL_W_DOOR_RT 0xB33
// hatches
#define CONTENT_WOOD_HATCH 0xB40
#define CONTENT_STEEL_HATCH 0xB42
#define CONTENT_WOOD_W_HATCH 0xB44
#define CONTENT_STEEL_W_HATCH 0xB46
// gates
#define CONTENT_WOOD_GATE 0xB60
#define CONTENT_STEEL_GATE 0xB62

// open doors
#define CONTENT_WOOD_DOOR_LB_OPEN 0xB80
#define CONTENT_WOOD_DOOR_LT_OPEN 0xB81
#define CONTENT_STEEL_DOOR_LB_OPEN 0xB82
#define CONTENT_STEEL_DOOR_LT_OPEN 0xB83
#define CONTENT_GLASS_DOOR_LB_OPEN 0xB84
#define CONTENT_GLASS_DOOR_LT_OPEN 0xB85
// open windowed doors
#define CONTENT_WOOD_W_DOOR_LB_OPEN 0xB90
#define CONTENT_WOOD_W_DOOR_LT_OPEN 0xB91
#define CONTENT_STEEL_W_DOOR_LB_OPEN 0xB92
#define CONTENT_STEEL_W_DOOR_LT_OPEN 0xB93
// open right doors
#define CONTENT_WOOD_DOOR_RB_OPEN 0xBA0
#define CONTENT_WOOD_DOOR_RT_OPEN 0xBA1
#define CONTENT_STEEL_DOOR_RB_OPEN 0xBA2
#define CONTENT_STEEL_DOOR_RT_OPEN 0xBA3
#define CONTENT_GLASS_DOOR_RB_OPEN 0xBA4
#define CONTENT_GLASS_DOOR_RT_OPEN 0xBA5
// open right windowed doors
#define CONTENT_WOOD_W_DOOR_RB_OPEN 0xBB0
#define CONTENT_WOOD_W_DOOR_RT_OPEN 0xBB1
#define CONTENT_STEEL_W_DOOR_RB_OPEN 0xBB2
#define CONTENT_STEEL_W_DOOR_RT_OPEN 0xBB3
// open hatches
#define CONTENT_WOOD_HATCH_OPEN 0xBC0
#define CONTENT_STEEL_HATCH_OPEN 0xBC2
#define CONTENT_WOOD_W_HATCH_OPEN 0xBC4
#define CONTENT_STEEL_W_HATCH_OPEN 0xBC6
// open gates
#define CONTENT_WOOD_GATE_OPEN 0xBE0
#define CONTENT_STEEL_GATE_OPEN 0xBE2
// door/hatch/gate masks
#define CONTENT_DOOR_MIN 0xB00
#define CONTENT_DOOR_MAX 0xBFF
#define CONTENT_DOOR_STEEL_MASK 0x002
#define CONTENT_DOOR_SECT_MASK 0x001
#define CONTENT_DOOR_OPEN_MASK 0x080
#define CONTENT_HATCH_MASK 0x040

#define CONTENT_SNOWMAN 0xC00
#define CONTENT_SNOW 0xC01
#define CONTENT_MUDSNOW 0xC02
#define CONTENT_SNOW_BLOCK 0xC03
#define CONTENT_ICE 0xC04
#define CONTENT_CONIFER_LEAVES 0xC06
#define CONTENT_CONIFER_SAPLING 0xC07
#define CONTENT_WOOD_PINE 0xC08
#define CONTENT_CONIFER_TREE 0xC09
// beds
#define CONTENT_BED_HEAD 0xC10
#define CONTENT_BED_FOOT 0xC11
#define CONTENT_BED_BLUE_HEAD 0xC12
#define CONTENT_BED_BLUE_FOOT 0xC13
#define CONTENT_BED_GREEN_HEAD 0xC14
#define CONTENT_BED_GREEN_FOOT 0xC15
#define CONTENT_BED_ORANGE_HEAD 0xC16
#define CONTENT_BED_ORANGE_FOOT 0xC17
#define CONTENT_BED_PURPLE_HEAD 0xC18
#define CONTENT_BED_PURPLE_FOOT 0xC19
#define CONTENT_BED_RED_HEAD 0xC1A
#define CONTENT_BED_RED_FOOT 0xC1B
#define CONTENT_BED_YELLOW_HEAD 0xC1C
#define CONTENT_BED_YELLOW_FOOT 0xC1D
#define CONTENT_BED_BLACK_HEAD 0xC1E
#define CONTENT_BED_BLACK_FOOT 0xC1F
#define CONTENT_BED_MIN 0xC10
#define CONTENT_BED_MAX 0xC1F
#define CONTENT_BED_FOOT_MASK 0x001
// FREE C20-CFF

// D00-DFF - play space for other devs and experiments

// FREE E00-FFF

#endif

