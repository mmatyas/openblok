#pragma once

#include "PieceType.h"
#include "system/Color.h"
#include "system/ResourceTypes.h"

#include <memory>
#include <unordered_map>


class GraphicsContext;
class Mino;

class MinoStorage {
public:
    static void loadTintedMinos(GraphicsContext&, const std::string&);
    static void loadTintedGhosts(GraphicsContext&, const std::string&);

    static std::shared_ptr<Mino> getMino(PieceType);
    static std::shared_ptr<Mino> getGhost(PieceType);

    static RGBColor color(PieceType);
    static char ascii(PieceType);

private:
    static std::unordered_map<PieceType, std::shared_ptr<Mino>, PieceTypeHash> minos;
    static std::unordered_map<PieceType, std::shared_ptr<Mino>, PieceTypeHash> ghosts;
};
