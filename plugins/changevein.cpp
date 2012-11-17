// Allow changing the material of a mineral inclusion

#include "Core.h"
#include "Console.h"
#include "Export.h"
#include "PluginManager.h"

#include "DataDefs.h"
#include "modules/Maps.h"
#include "modules/Materials.h"
#include "df/matgloss_stone.h"
#include "TileTypes.h"

using std::vector;
using std::string;
using namespace DFHack;
using namespace df::enums;

using df::global::world;
using df::global::cursor;

command_result df_changevein (color_ostream &out, vector <string> & parameters)
{
    if (parameters.size() != 1)
        return CR_WRONG_USAGE;

    CoreSuspender suspend;

    if (!Maps::IsValid())
    {
        out.printerr("Map is not available!\n");
        return CR_FAILURE;
    }
    if (!cursor || cursor->x == -30000)
    {
        out.printerr("No cursor detected - please place the cursor over a mineral vein.\n");
        return CR_FAILURE;
    }

    MaterialInfo mi;
    if (!mi.findStone(material_type::STONE, parameters[0]))
    {
        out.printerr("No such material!\n");
        return CR_FAILURE;
    }
    if (mi.stone->flags.is_set(matgloss_stone_flags::SOIL_ANY))
    {
        out.printerr("Invalid material - you must select a type of stone or gem\n");
        return CR_FAILURE;
    }

    df::map_block *block = Maps::getTileBlock(cursor->x, cursor->y, cursor->z);
    if (!block)
    {
        out.printerr("Invalid tile selected.\n");
        return CR_FAILURE;
    }
    df::block_square_event_mineralst *mineral = NULL;
    int tx = cursor->x % 16, ty = cursor->y % 16;
    for (size_t j = 0; j < block->block_events.size(); j++)
    {
        df::block_square_event *evt = block->block_events[j];
        if (evt->getType() != block_square_event_type::mineral)
            continue;
        df::block_square_event_mineralst *cur = (df::block_square_event_mineralst *)evt;
        if (cur->getassignment(tx, ty))
            mineral = cur;
    }
    if (!mineral)
    {
        out.printerr("Selected tile does not contain a mineral vein.\n");
        return CR_FAILURE;
    }
    mineral->stone = mi.subtype;

    return CR_OK;
}

DFHACK_PLUGIN("changevein");

DFhackCExport command_result plugin_init ( color_ostream &out, std::vector <PluginCommand> &commands)
{
    commands.push_back(PluginCommand("changevein",
        "Changes the material of a mineral inclusion.",
        df_changevein, false,
        "Syntax: changevein <inorganic material ID>\n"));
    return CR_OK;
}

DFhackCExport command_result plugin_shutdown ( color_ostream &out )
{
    return CR_OK;
}
