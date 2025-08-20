#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include <mpi.h>
//#include "scorep/SCOREP_User.h"

#include "life.h"

struct data{
    const int N; // NOLINT
    std::vector<std::vector<int>> field;
    std::vector<int> boundaryN;
    std::vector<int> boundaryE;
    std::vector<int> boundaryS;
    std::vector<int> boundaryW;
    std::random_device rd;
    std::mt19937 gen;

    data(const int n, const int init = 0) :
            N(n),
            field(n, std::vector<int>(n, init)),
            boundaryN(n, init),
            boundaryE(n, init),
            boundaryS(n, init),
            boundaryW(n, init),
            gen(rd())
    {
        //SCOREP_USER_REGION( "<data>", SCOREP_USER_REGION_TYPE_FUNCTION )
        std::uniform_int_distribution<> dis(0, 4);

        for(auto& v : field) {
            for(auto& cell : v) {
                cell = dis(gen);
            }
        }
    }
};

bool isSquare(const int nprocs){
    const int MAX_SIZE = 32; // 1024 processes
    for(unsigned int i = 1; i < MAX_SIZE; i++){
        unsigned int square = i*i;
        if(square == nprocs) {
            return true;
        }
    }
    return false;
}

int swapBoundaries(MPI_Comm cart, const int rank, data* d) {
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays)
    //SCOREP_USER_REGION( "<swap>", SCOREP_USER_REGION_TYPE_FUNCTION )
    int sourceE = -1;
    int neighborE = -1;
    MPI_Cart_shift(cart, 1, 1, &sourceE, &neighborE);
    int sourceN = -1;
    int neighborN = -1;
    MPI_Cart_shift(cart, 0, 1, &sourceN, &neighborN);
    const int neighborS = sourceN; // NOLINT
    const int neighborW = sourceE; // NOLINT
    /* std::cout << "rank: " << rank << " Neighbors N,E,S,W are: " << neighborN << " " << neighborE << " " << neighborS << " " << neighborW << '\n'; */

    int source, dest; // NOLINT
    int sendbuf[d->N]; // NOLINT
    const int sendcount = d->N;
    int sendtag = 0;

    int recvbuf[d->N]; //NOLINT
    const int recvcount = d->N;
    int recvtag = 0;

    // North
    for(unsigned int i = 0; i < d->N; i++){
        // Send to north
        sendbuf[i] = d->field[0][i];
    }

    MPI_Cart_shift(cart, 0, 1, &source, &dest);
    MPI_Sendrecv(sendbuf, sendcount, MPI_INT, dest, sendtag,
                 recvbuf, recvcount, MPI_INT, source, recvtag,
                 cart, MPI_STATUS_IGNORE);

    for(unsigned int i = 0; i < d->N; i++){
        // Receive from south
        d->boundaryS[i] = recvbuf[i];
    }

    // South
    for(unsigned int i = 0; i < d->N; i++){
        // Send to south
        sendbuf[i] = d->field[d->N-1][i];
    }

    MPI_Cart_shift(cart, 0, -1, &source, &dest);
    MPI_Sendrecv(sendbuf, sendcount, MPI_INT, dest, sendtag,
                 recvbuf, recvcount, MPI_INT, source, recvtag,
                 cart, MPI_STATUS_IGNORE);

    for(unsigned int i = 0; i < d->N; i++){
        // Receive from north
        d->boundaryN[i] = recvbuf[i];
    }

    // East
    for(unsigned int i = 0; i < d->N; i++){
        // Send to east
        sendbuf[i] = d->field[i][d->N-1];
    }

    MPI_Cart_shift(cart, 1, 1, &source, &dest);
    MPI_Sendrecv(sendbuf, sendcount, MPI_INT, dest, sendtag,
                 recvbuf, recvcount, MPI_INT, source, recvtag,
                 cart, MPI_STATUS_IGNORE);

    for(unsigned int i = 0; i < d->N; i++){
        // Receive from west
        d->boundaryW[i] = recvbuf[i];
    }

    // West
    for(unsigned int i = 0; i < d->N; i++){
        // Send to west
        sendbuf[i] = d->field[i][0];
    }

    MPI_Cart_shift(cart, 1, -1, &source, &dest);
    MPI_Sendrecv(sendbuf, sendcount, MPI_INT, dest, sendtag,
                 recvbuf, recvcount, MPI_INT, source, recvtag,
                 cart, MPI_STATUS_IGNORE);

    for(unsigned int i = 0; i < d->N; i++){
        // Receive from east
        d->boundaryE[i] = recvbuf[i];
    }

    // NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays)
    return 0;
}

void printData(data *d, const int window = 0){
    int xlow = 0;
    int xhigh = d->N;
    int ylow = 0;
    int yhigh = d->N;
    if(window > 0) {
        xhigh = window;
        yhigh = window;
    }
    const int width = 5;
    std::cout << '\n';

    if(window == 0){
        std::cout << std::setw(width) << "";
        for(unsigned int i = xlow; i < xhigh; i++){
            std::cout << std::setw(width) << d->boundaryN[i]; // NOLINT
        }
        std::cout << '\n';
    }

    std::cout << '\n';
    for(unsigned int i = xlow; i < xhigh; i++){
        std::cout << std::setw(width) << d->boundaryW[i]; // NOLINT
        for(unsigned int j = ylow; j < yhigh; j++){
            std::cout << std::setw(width) << d->field[i][j]; // NOLINT
        }
        if(window == 0){
            std::cout << std::setw(width) << d->boundaryE[i];
        }

        std::cout << '\n';
    }

    std::cout << '\n';
    std::cout << std::setw(width) << "";
    for(unsigned int i = xlow; i < xhigh; i++){
        std::cout << std::setw(width) << d->boundaryS[i];
    }
    std::cout << '\n';

    std::cout << '\n';

    //using namespace std::chrono_literals;
    //std::this_thread::sleep_for(250ms);
}

int applyRules(const int N, const int E, const int S, const int W, const int C, double random){ // NOLINT
    //SCOREP_USER_REGION( "<applyRules>", SCOREP_USER_REGION_TYPE_FUNCTION )
    int newC = C;
    int neighborsabove = 0;
    int neighborsbelow = 0;
    if( N > C ){
        neighborsabove += 1;
    } else if (N < C) {
        neighborsbelow += 1;
    }
    if( E > C ){
        neighborsabove += 1;
    } else if (E < C) {
        neighborsbelow += 1;
    }
    if( S > C ){
        neighborsabove += 1;
    } else if (S < C) {
        neighborsbelow += 1;
    }
    if( W > C ){
        neighborsabove += 1;
    } else if (W < C) {
        neighborsbelow += 1;
    }

    double likelyness = 0.01; // NOLINT

    if(neighborsabove != neighborsbelow){
        newC += (-2*((N==0)*(E==0)*(S==0)*(W==0) || (neighborsabove > neighborsbelow))) + 1;
    } else {
        likelyness = 0.45; // NOLINT
    }

    if( random < likelyness) {
        newC += 1;
    } else if (random > (1-likelyness)) {
        newC -= 1;
    }

    if(newC < 0 || newC > 9999){ // NOLINT
        newC = 0;
    }

    return newC;
}

void updateField(data *d){
    //SCOREP_USER_REGION( "<updateField>", SCOREP_USER_REGION_TYPE_FUNCTION )
    int N = 0;
    int E = 0;
    int S = 0;
    int W = 0;
    int C = 0;

    std::uniform_real_distribution<> dis(0.0, 1.0);
    double random = 0.0;

    // Generate many numbers efficiently
    std::vector<double> random_numbers;
    random_numbers.reserve(d->N*d->N);
    for(int i = 0; i < d->N*d->N; ++i) {
        random_numbers.push_back(dis(d->gen));
    }

    for(unsigned int i = 0; i < d->N; i++){
        for(unsigned int j = 0; j < d->N; j++){
            if(i == 0){
                W = d->boundaryW[j];
                E = d->field[i+1][j];
            } else if(i == (d->N-1)) {
                W = d->field[i-1][j];
                E = d->boundaryE[j];
            } else {
                W = d->field[i-1][j];
                E = d->field[i+1][j];
            }

            if(j == 0){
                S = d->boundaryS[i];
                N = d->field[i][j+1];
            } else if(j == (d->N-1)) {
                S = d->field[i][j-1];
                N = d->boundaryN[i];
            } else {
                S = d->field[i][j-1];
                N = d->field[i][j+1];
            }
            C = d->field[i][j];
            d->field[i][j] = applyRules(N, E, S, W, C, random_numbers[i+d->N*j]);
        }
    }
}

int main(int argc, char* argv[]){
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-magic-numbers)
    MPI_Init(&argc, &argv);
    int num_procs = 0;
    int rank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int N = 1000;
    int timesteps = 1000;

    if(rank == 0) {
        if(! isSquare(num_procs)) {
            std::cout << "Start with square numbers of processes (1, 4, 9, 16, ...)!\n";
            return 193; // NOLINT
        }
    }

    const int ndim = 2;
    const int length = static_cast<int>(std::lround(std::sqrt(num_procs)));
    const int dims[ndim] = {length, length};
    const int periods[ndim] = {1, 1};
    MPI_Comm cart; // NOLINT
    MPI_Cart_create(MPI_COMM_WORLD, ndim, dims, periods, 0, &cart);

    int mycoordinates[ndim] = {};
    MPI_Cart_coords(cart, rank, ndim, mycoordinates);
    std::cout << "rank: " << rank << " coords: " << mycoordinates[0] << " " << mycoordinates[1] << '\n';

    // Prepare data
    auto d = std::make_unique<data>( N );

    if(rank == 1){
        printData(d.get(), 20);
    }

    // Run timesteps

    for(unsigned int i = 0; i < timesteps; i++){
        swapBoundaries(cart, rank, d.get());
        updateField(d.get());
    }

    if(rank == 1){
        printData(d.get(), 20);
    }

    MPI_Finalize();
    // NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-magic-numbers)

    return 0;
}
