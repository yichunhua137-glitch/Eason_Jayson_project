module;

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

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

string tileValueLabel(const Board &board, int tileId) {
    const Tile &tile = board.getTile(tileId);

    if (tile.getType() == ResourceType::PARK) {
        return "  ";
    }

    return label2(tile.getNumber());
}

struct Position {
    int x;
    int y;
};

void put(
    vector<string> &canvas,
    int x,
    int y,
    const string &text
) {
    if (y < 0 || y >= static_cast<int>(canvas.size())) {
        return;
    }

    for (int i = 0; i < static_cast<int>(text.size()); ++i) {
        int column = x + i;

        if (
            column >= 0 &&
            column < static_cast<int>(canvas[y].size())
        ) {
            canvas[y][column] = text[i];
        }
    }
}

Position vertexPosition(int vertexId) {
    if (vertexId < 2) {
        return Position{30 + 10 * vertexId, 0};
    }

    if (vertexId < 6) {
        return Position{20 + 10 * (vertexId - 2), 4};
    }

    if (vertexId < 48) {
        int row = (vertexId - 6) / 6;
        int column = (vertexId - 6) % 6;
        return Position{10 + 10 * column, 8 + 4 * row};
    }

    if (vertexId < 52) {
        return Position{20 + 10 * (vertexId - 48), 36};
    }

    return Position{30 + 10 * (vertexId - 52), 40};
}

Position tilePosition(int tileId) {
    static const Position positions[19] = {
        {35, 2},
        {25, 6}, {45, 6},
        {15, 10}, {35, 10}, {55, 10},
        {25, 14}, {45, 14},
        {15, 18}, {35, 18}, {55, 18},
        {25, 22}, {45, 22},
        {15, 26}, {35, 26}, {55, 26},
        {25, 30}, {45, 30},
        {35, 34}
    };

    return positions[tileId];
}

Position edgeEndpointPosition(int edgeId, int endpoint) {
    static const int endpoints[72][2] = {
        {0, 1}, {0, 3}, {1, 4}, {2, 3}, {4, 5}, {2, 7},
        {3, 8}, {4, 9}, {5, 10}, {6, 7}, {8, 9}, {10, 11},
        {6, 12}, {7, 13}, {8, 14}, {9, 15}, {10, 16}, {11, 17},
        {13, 14}, {15, 16}, {12, 18}, {13, 19}, {14, 20},
        {15, 21}, {16, 22}, {17, 23}, {18, 19}, {20, 21},
        {22, 23}, {18, 24}, {19, 25}, {20, 26}, {21, 27},
        {22, 28}, {23, 29}, {25, 26}, {27, 28}, {24, 30},
        {25, 31}, {26, 32}, {27, 33}, {28, 34}, {29, 35},
        {30, 31}, {32, 33}, {34, 35}, {30, 36}, {31, 37},
        {32, 38}, {33, 39}, {34, 40}, {35, 41}, {37, 38},
        {39, 40}, {36, 42}, {37, 43}, {38, 44}, {39, 45},
        {40, 46}, {41, 47}, {42, 43}, {44, 45}, {46, 47},
        {43, 48}, {44, 49}, {45, 50}, {46, 51}, {48, 49},
        {50, 51}, {49, 52}, {50, 53}, {52, 53}
    };

    return vertexPosition(endpoints[edgeId][endpoint]);
}

} // namespace

TextDisplay::TextDisplay(const Board &board)
    : board{board} {}

string TextDisplay::render() const {
    return renderFormal();
}

string TextDisplay::renderFormal() const {
    vector<string> canvas(41, string(74, ' '));

    // Draw the 72 roads from the fixed Figure 3 topology. Horizontal roads
    // use "--xx--"; vertical roads use the label between bars.
    for (int edgeId = 0; edgeId < 72; ++edgeId) {
        Position first = edgeEndpointPosition(edgeId, 0);
        Position second = edgeEndpointPosition(edgeId, 1);

        if (first.y == second.y) {
            if (first.x > second.x) {
                Position temporary = first;
                first = second;
                second = temporary;
            }

            put(
                canvas,
                first.x + 4,
                first.y,
                "--" + edgeLabel(board, edgeId) + "--"
            );
        } else {
            if (first.y > second.y) {
                Position temporary = first;
                first = second;
                second = temporary;
            }

            put(canvas, first.x, first.y + 1, "|");
            put(
                canvas,
                first.x - 1,
                first.y + 2,
                edgeLabel(board, edgeId)
            );
            put(canvas, first.x, first.y + 3, "|");
        }
    }

    // Vertices are drawn after roads so their boundary bars are never
    // overwritten at road intersections.
    for (int vertexId = 0; vertexId < 54; ++vertexId) {
        Position position = vertexPosition(vertexId);
        put(
            canvas,
            position.x,
            position.y,
            "|" + vertexLabel(board, vertexId) + "|"
        );
    }

    // Each tile uses the three central lines shown in ctor.pdf Figure 3.
    // GEESE occupies the otherwise blank line immediately below the value,
    // matching Figure 2, without changing the shape of the board.
    for (int tileId = 0; tileId < 19; ++tileId) {
        Position position = tilePosition(tileId);
        string resource =
            resourceName(board.getTile(tileId).getType());

        put(
            canvas,
            position.x - 1,
            position.y,
            label2(tileId)
        );
        put(
            canvas,
            position.x -
                static_cast<int>(resource.size()) / 2,
            position.y + 1,
            resource
        );
        put(
            canvas,
            position.x - 1,
            position.y + 2,
            tileValueLabel(board, tileId)
        );

        if (board.getTile(tileId).hasGeeseOnTile()) {
            put(
                canvas,
                position.x - 2,
                position.y + 3,
                "GEESE"
            );
        }
    }

    ostringstream out;

    for (string line : canvas) {
        size_t last = line.find_last_not_of(' ');

        if (last == string::npos) {
            out << '\n';
        } else {
            out << line.substr(0, last + 1) << '\n';
        }
    }

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
            << " value=" << setw(5) << left
            << tileValueLabel(board, tileId)
            << " geese="
            << (tile.hasGeeseOnTile() ? "yes" : "no")
            << " vertices=";

        for (int i = 0;
             i < static_cast<int>(vertices.size());
             ++i) {
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
