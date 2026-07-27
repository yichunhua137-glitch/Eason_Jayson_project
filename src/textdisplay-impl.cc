module;

#include <iomanip>
#include <sstream>
#include <string>

module textdisplay;

import edge;
import residence;
import tile;
import types;
import vertex;

using namespace std;

namespace {

char colourCode(Colour colour) {
    if (colour == Colour::BLUE) {
        return 'B';
    } else if (colour == Colour::RED) {
        return 'R';
    } else if (colour == Colour::ORANGE) {
        return 'O';
    } else if (colour == Colour::YELLOW) {
        return 'Y';
    }

    return 'N';
}

string resourceName(ResourceType type) {
    if (type == ResourceType::BRICK) {
        return "BRICK";
    } else if (type == ResourceType::ENERGY) {
        return "ENERGY";
    } else if (type == ResourceType::GLASS) {
        return "GLASS";
    } else if (type == ResourceType::HEAT) {
        return "HEAT";
    } else if (type == ResourceType::WIFI) {
        return "WIFI";
    }

    return "PARK";
}

char residenceCode(ResidenceType type) {
    if (type == ResidenceType::BASEMENT) {
        return 'B';
    } else if (type == ResidenceType::HOUSE) {
        return 'H';
    }

    return 'T';
}

string label2(int value) {
    ostringstream out;
    out << setw(2) << value;
    return out.str();
}

string vertexLabel(const Board &board, int vertexId) {
    const Vertex &vertex = board.getVertex(vertexId);
    ostringstream out;

    if (vertex.hasResidence()) {
        Residence *residence = vertex.getResidence();
        out << colourCode(residence->getOwner())
            << residenceCode(residence->getType());
    } else {
        out << setw(2) << vertexId;
    }

    return out.str();
}

string edgeLabel(const Board &board, int edgeId) {
    const Edge &edge = board.getEdge(edgeId);
    ostringstream out;

    if (edge.isBuilt()) {
        out << colourCode(edge.getOwner()) << 'R';
    } else {
        out << setw(2) << edgeId;
    }

    return out.str();
}

string tileIdLabel(int tileId) {
    return label2(tileId);
}

string tileValueLabel(const Board &board, int tileId) {
    const Tile &tile = board.getTile(tileId);

    if (tile.getType() == ResourceType::PARK) {
        return "  ";
    }

    return label2(tile.getNumber());
}

string geeseLabel(const Board &board, int tileId) {
    if (board.getTile(tileId).hasGeeseOnTile()) {
        return "GEESE";
    }

    return "     ";
}

void appendTileBlock(
    ostringstream &out,
    const Board &board,
    int tileId,
    int leftEdge,
    int topLeftVertex,
    int topRightVertex,
    int bottomLeftVertex,
    int bottomRightVertex,
    const string &indent
) {
    out << indent
        << "|" << vertexLabel(board, topLeftVertex) << "|--"
        << edgeLabel(board, leftEdge) << "--|"
        << vertexLabel(board, topRightVertex) << "|\n";

    out << indent << "|               |\n";

    out << indent << "    " << tileIdLabel(tileId) << "         \n";

    {
        ostringstream line;
        line << indent << "   " << setw(7) << left
             << resourceName(board.getTile(tileId).getType())
             << "     ";
        out << line.str() << '\n';
    }

    out << indent << "    " << tileValueLabel(board, tileId) << "         \n";

    if (geeseLabel(board, tileId) != "     ") {
        out << indent << "  " << geeseLabel(board, tileId) << "      \n";
    }

    out << indent
        << "|" << vertexLabel(board, bottomLeftVertex) << "|     |"
        << vertexLabel(board, bottomRightVertex) << "|\n";
}

} // namespace

TextDisplay::TextDisplay(const Board &board)
    : board{board} {}

string TextDisplay::render() const {
    return renderFormal();
}

string TextDisplay::renderFormal() const {
    ostringstream out;

    out << "                |" << vertexLabel(board, 0) << "|--"
        << edgeLabel(board, 0) << "--|" << vertexLabel(board, 1) << "|\n";
    out << "                |         |\n";
    out << "                   " << tileIdLabel(0) << '\n';
    out << "                 BRICK\n";
    out << "                   " << tileValueLabel(board, 0) << '\n';
    out << "          |" << vertexLabel(board, 2) << "|--"
        << edgeLabel(board, 3) << "--|" << vertexLabel(board, 3)
        << "|   " << tileIdLabel(1) << "   |" << vertexLabel(board, 4)
        << "|--" << edgeLabel(board, 4) << "--|" << vertexLabel(board, 5)
        << "|\n";
    out << "          |         |       |         |\n";
    out << "             " << tileIdLabel(1) << "         " << tileIdLabel(2) << '\n';
    out << "           ENERGY        HEAT\n";
    out << "             " << tileValueLabel(board, 1) << "         "
        << tileValueLabel(board, 2) << '\n';
    out << "|" << vertexLabel(board, 6) << "|--" << edgeLabel(board, 9)
        << "--|" << vertexLabel(board, 7) << "|   " << tileIdLabel(3)
        << "   |" << vertexLabel(board, 8) << "|--" << edgeLabel(board, 10)
        << "--|" << vertexLabel(board, 9) << "|   " << tileIdLabel(5)
        << "   |" << vertexLabel(board, 10) << "|--" << edgeLabel(board, 11)
        << "--|" << vertexLabel(board, 11) << "|\n";
    out << "|         |       |         |       |         |\n";
    out << "   " << tileIdLabel(3) << "         " << tileIdLabel(4)
        << "         " << tileIdLabel(5) << '\n';
    out << " ENERGY        PARK         HEAT\n";
    out << "   " << tileValueLabel(board, 3) << "         "
        << tileValueLabel(board, 4) << "         "
        << tileValueLabel(board, 5) << '\n';
    if (geeseLabel(board, 4) != "     ") {
        out << "             " << geeseLabel(board, 4) << '\n';
    }
    out << "|" << vertexLabel(board, 12) << "|--" << edgeLabel(board, 12)
        << "--|" << vertexLabel(board, 13) << "|--" << edgeLabel(board, 18)
        << "--|" << vertexLabel(board, 14) << "|--" << edgeLabel(board, 19)
        << "--|" << vertexLabel(board, 16) << "|--" << edgeLabel(board, 17)
        << "--|" << vertexLabel(board, 17) << "|\n";
    out << "|         |       |         |       |         |\n";
    out << "     " << tileIdLabel(6) << "         " << tileIdLabel(7)
        << "         " << tileIdLabel(8) << '\n';
    out << "     GLASS        BRICK       BRICK\n";
    out << "      " << tileValueLabel(board, 6) << "         "
        << tileValueLabel(board, 7) << "         "
        << tileValueLabel(board, 8) << '\n';
    out << "|" << vertexLabel(board, 18) << "|--" << edgeLabel(board, 26)
        << "--|" << vertexLabel(board, 19) << "|   " << tileIdLabel(8)
        << "   |" << vertexLabel(board, 20) << "|--" << edgeLabel(board, 27)
        << "--|" << vertexLabel(board, 21) << "|   " << tileIdLabel(10)
        << "   |" << vertexLabel(board, 22) << "|--" << edgeLabel(board, 28)
        << "--|" << vertexLabel(board, 23) << "|\n";
    out << "|         |       |         |       |         |\n";
    out << "   " << tileIdLabel(8) << "         " << tileIdLabel(9)
        << "         " << tileIdLabel(10) << '\n';
    out << "   HEAT         BRICK       BRICK\n";
    out << "    " << tileValueLabel(board, 8) << "         "
        << tileValueLabel(board, 9) << "         "
        << tileValueLabel(board, 10) << '\n';
    out << "|" << vertexLabel(board, 24) << "|--" << edgeLabel(board, 24)
        << "--|" << vertexLabel(board, 25) << "|--" << edgeLabel(board, 35)
        << "--|" << vertexLabel(board, 26) << "|--" << edgeLabel(board, 36)
        << "--|" << vertexLabel(board, 28) << "|--" << edgeLabel(board, 34)
        << "--|" << vertexLabel(board, 29) << "|\n";
    out << "|         |       |         |       |         |\n";
    out << "     " << tileIdLabel(11) << "         " << tileIdLabel(12)
        << "         " << tileIdLabel(13) << '\n';
    out << "    ENERGY        WIFI        GLASS\n";
    out << "      " << tileValueLabel(board, 11) << "         "
        << tileValueLabel(board, 12) << "         "
        << tileValueLabel(board, 13) << '\n';
    out << "|" << vertexLabel(board, 30) << "|--" << edgeLabel(board, 43)
        << "--|" << vertexLabel(board, 31) << "|   " << tileIdLabel(13)
        << "   |" << vertexLabel(board, 32) << "|--" << edgeLabel(board, 44)
        << "--|" << vertexLabel(board, 33) << "|   " << tileIdLabel(15)
        << "   |" << vertexLabel(board, 34) << "|--" << edgeLabel(board, 45)
        << "--|" << vertexLabel(board, 35) << "|\n";
    out << "|         |       |         |       |         |\n";
    out << "   " << tileIdLabel(13) << "         " << tileIdLabel(14)
        << "         " << tileIdLabel(15) << '\n';
    out << "   ENERGY        WIFI        GLASS\n";
    out << "    " << tileValueLabel(board, 13) << "         "
        << tileValueLabel(board, 14) << "         "
        << tileValueLabel(board, 15) << '\n';
    out << "|" << vertexLabel(board, 36) << "|--" << edgeLabel(board, 36)
        << "--|" << vertexLabel(board, 37) << "|--" << edgeLabel(board, 52)
        << "--|" << vertexLabel(board, 38) << "|--" << edgeLabel(board, 53)
        << "--|" << vertexLabel(board, 40) << "|--" << edgeLabel(board, 41)
        << "--|" << vertexLabel(board, 41) << "|\n";
    out << "|         |       |         |       |         |\n";
    out << "     " << tileIdLabel(16) << "         " << tileIdLabel(17)
        << "         \n";
    out << "      WIFI         GLASS\n";
    out << "       " << tileValueLabel(board, 16) << "         "
        << tileValueLabel(board, 17) << '\n';
    out << "          |" << vertexLabel(board, 42) << "|--"
        << edgeLabel(board, 60) << "--|" << vertexLabel(board, 43)
        << "|   " << tileIdLabel(18) << "   |" << vertexLabel(board, 44)
        << "|--" << edgeLabel(board, 61) << "--|" << vertexLabel(board, 45)
        << "|\n";
    out << "          |         |       |         |\n";
    out << "             " << tileIdLabel(18) << "         \n";
    out << "            GLASS\n";
    out << "             " << tileValueLabel(board, 18) << '\n';
    out << "                |" << vertexLabel(board, 48) << "|--"
        << edgeLabel(board, 67) << "--|" << vertexLabel(board, 49)
        << "|--" << edgeLabel(board, 68) << "--|" << vertexLabel(board, 51)
        << "|\n";
    out << "                      |         |\n";
    out << "                         |" << vertexLabel(board, 52)
        << "|--" << edgeLabel(board, 71) << "--|"
        << vertexLabel(board, 53) << "|\n";

    return out.str();
}

string TextDisplay::renderDebug() const {
    ostringstream out;

    out << "Board State\n";
    out << "===========\n";
    out << '\n';

    out << "Tiles\n";
    out << "-----\n";
    for (int tileId = 0; tileId < 19; ++tileId) {
        const Tile &tile = board.getTile(tileId);
        const auto &vertices = board.getVerticesForTile(tileId);

        out << '[' << setw(2) << tileId << "] "
            << setw(7) << left << resourceName(tile.getType())
            << " value=" << setw(5) << left << tileValueLabel(board, tileId)
            << " geese=" << (tile.hasGeeseOnTile() ? "yes" : "no")
            << " vertices=";

        for (int i = 0; i < static_cast<int>(vertices.size()); ++i) {
            if (i > 0) {
                out << ',';
            }
            out << vertices[i];
        }

        out << '\n';
    }

    out << '\n';
    out << "Vertices\n";
    out << "--------\n";
    for (int vertexId = 0; vertexId < 54; ++vertexId) {
        out << '[' << setw(2) << vertexId << "] "
            << vertexLabel(board, vertexId);

        if ((vertexId + 1) % 6 == 0 || vertexId == 53) {
            out << '\n';
        } else {
            out << "    ";
        }
    }

    out << '\n';
    out << "Edges\n";
    out << "-----\n";
    for (int edgeId = 0; edgeId < 72; ++edgeId) {
        const auto &ends = board.getVerticesForEdge(edgeId);

        out << '[' << setw(2) << edgeId << "] "
            << edgeLabel(board, edgeId)
            << " ("
            << ends[0] << ','
            << ends[1] << ')';

        if ((edgeId + 1) % 4 == 0 || edgeId == 71) {
            out << '\n';
        } else {
            out << "    ";
        }
    }

    return out.str();
}

ostream &operator<<(
    ostream &out,
    const TextDisplay &display
) {
    out << display.render();
    return out;
}
