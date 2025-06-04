#ifndef LIFE_H
#define LIFE_H

namespace LIFE {
    struct coord {
        int x;
        int y;
    };

    const coord getN(coord const& C) {
        auto north = coord{C.x, C.y+1};
        return north;
    }

    const coord getS(coord const& C) {
        auto south = coord{C.x, C.y-1};
        return south;
    }

    const coord getE(const coord& C) {
        auto east = coord{C.x+1, C.y};
        return east;
    }

    const coord getW(const coord& C) {
        auto west = coord{C.x-1, C.y};
        return west;
    }

    const coord getNW(const coord& C){
        return getW(getN(C));
    }

    const coord getNE(const coord& C){
        return getE(getN(C));
    }

    const coord getSW(const coord& C){
        return getW(getS(C));
    }

    const coord getSE(const coord& C){
        return getE(getS(C));
    }
};

#endif // LIFE_H
