#pragma once

#include "PieceType.h"
#include "system/Color.h"

#include <memory>
#include <unordered_map>

class AppContext;
class GraphicsContext;
class Mino;


class MinoStorage {
public:
    static void loadMinos(AppContext&);
    static void loadGhosts(AppContext&);
    static void loadMatrixCell(GraphicsContext&, const std::string&);

    static std::shared_ptr<Mino> getMino(PieceType);
    static std::shared_ptr<Mino> getGhost(PieceType);
    static std::shared_ptr<Mino> getMatrixCell();

    static RGBColor color(PieceType);

#ifndef NDEBUG
    static void loadDummyMinos();
#endif

private:
    static void loadTintedMinos(GraphicsContext&, const std::string&);
    static void loadCustomMinos(AppContext&);
    static void loadTintedGhosts(GraphicsContext&, const std::string&);
    static void loadSimpleGhosts(GraphicsContext&, const std::string&);
    static std::unordered_map<PieceType, std::shared_ptr<Mino>, PieceTypeHash> minos;
    static std::unordered_map<PieceType, std::shared_ptr<Mino>, PieceTypeHash> ghosts;
    static std::shared_ptr<Mino> matrixcell;
};
