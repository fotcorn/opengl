#include <vector>

struct Voxel {
    enum class Type { WATER, GRAS, STONE } type;
    bool visible = false;
    Voxel(Type type) {
        this->type = type;
    }
};

struct Position {
    int x, y, z;
};

class VoxelWorld {
    Voxel* world[10][10][10];

public:
    VoxelWorld() {
        for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 10; y++) {
                for (int z = 0; z < 10; z++) {
                    if (y == 0 || y == 1) {
                        world[x][y][z] = new Voxel(Voxel::Type::STONE);
                    } else {
                        world[x][y][z] = nullptr;
                    }
                }
            }
        }
    }

    void calc() {
        std::vector<Position> gras;
        std::vector<Position> stone;
        std::vector<Position> water;

        // TODO
    }
    void draw() {
        // use shader
        // bind grad positions to attribute
        // bind green color to program
        // draw glDrawArraysInstanced on cube
    }
};
