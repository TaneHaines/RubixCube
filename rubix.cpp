#include <iostream>
#include <limits>
#include <ios>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

struct Vec3 {
    int x, y, z;

    Vec3(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}

    bool operator==(const Vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    int face_index() const {
        if (*this == Vec3(0, 0, 1))  return 0; // Front
        if (*this == Vec3(0, 0, -1)) return 2; // Back
        if (*this == Vec3(0, 1, 0))  return 4; // Top
        if (*this == Vec3(0, -1, 0)) return 5; // Bottom
        if (*this == Vec3(-1, 0, 0)) return 1; // Left
        if (*this == Vec3(1, 0, 0))  return 3; // Right
        return -1; // Invalid
    }

    void print() const {
        cout << "(" << x << ", " << y << ", " << z << ")";
    }
};

struct Camera {
    Vec3 forward, up, right;

    Camera() {
        forward = Vec3(0, 0, 1); 
        up      = Vec3(0, 1, 0); 
        right   = Vec3(1, 0, 0); 
    }

    template<typename T>
    void rotate_90(T& a, T& b, int direction) {
        if (direction == 1) {
            const T tmp = a;
            a = T(-b.x, -b.y, -b.z);
            b = tmp;
        } else {
            T tmp = a;
            a = b;
            b = T(-tmp.x, -tmp.y, -tmp.z);
        }
    }

    void rotate_x(int direction) {
        rotate_90(forward, right, direction);
    }

    void rotate_y(int direction) {
        rotate_90(forward, up, direction);
    }

    void rotate_z(int direction) {
        rotate_90(up, right, direction);
    }

    int face_index() {
        return forward.face_index();
    }
};

template<typename T, size_t N>
void rotate_clockwise(T (&values)[N]) {
    size_t dim = static_cast<size_t>(sqrt(N));  
    T temp[N];  
    std::copy(std::begin(values), std::end(values), std::begin(temp));

    for (size_t row = 0; row < dim; ++row) {
        for (size_t col = 0; col < dim; ++col) {
            values[col * dim + (dim - row - 1)] = temp[row * dim + col];
        }
    }
}

template<typename T, size_t N>
void rotate_counterclockwise(T (&values)[N]) {
    size_t dim = static_cast<size_t>(sqrt(N)); 
    T temp[N]; 
    std::copy(std::begin(values), std::end(values), std::begin(temp));

    for (size_t row = 0; row < dim; ++row) {
        for (size_t col = 0; col < dim; ++col) {
            values[(dim - col - 1) * dim + row] = temp[row * dim + col];
        }
    }
}

template<typename T, size_t N>
struct Side { 
    T values[N];

    void rotate_clockwise() {
        ::rotate_clockwise(values);
    }

    void rotate_counterclockwise() {
        ::rotate_counterclockwise(values);
    }
};

template<typename T, size_t N> struct Input { T input[N] = {}; };

template<typename T, size_t N>
void safe_cin(Input<T, N>& input) {
    while (true) {
        cin.get(input.input, N);
        if (cin.fail()) {
            cin.clear(); // Clears error flags.
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please try again: ";
        } else if (input.input[0] == '\0') {
            cout << "No input detected. Please enter a command: ";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            break;
        }
    }
}

template<typename T, size_t S, size_t N> 
class Cube { 
    Side<T, N> sides[S];
    Input<char, 3> input;

    const size_t dim = static_cast<size_t>(sqrt(N));
    Camera camera = Camera();

    public:
        Cube() {}

        void init() {
            for (int i = 0; i < S; ++i) {
                for (int j = 0; j < N; ++j) {
                    sides[i].values[j] = static_cast<T>(i);
                }
            }
        }

        void rotate_adjacent_sides(int face_index, int direction) {
            T temp_row[dim];

            // Handle adjacent rotations dynamically
            // We assume that when rotating any face, the adjacent sides are affected
            if (direction == 1) { // Clockwise
                switch (face_index) {
                    case 0: // Front
                        // Rotate the top, left, bottom, and right sides
                        std::copy(sides[4].values + dim * (dim - 1), sides[4].values + dim * dim, temp_row); // Top row of top
                        std::copy(sides[1].values + dim * 0, sides[1].values + dim, sides[4].values + dim * (dim - 1)); // Left -> Top
                        std::copy(sides[5].values + dim * 0, sides[5].values + dim, sides[1].values + dim * 0); // Bottom -> Left
                        std::copy(sides[2].values + dim * (dim - 1), sides[2].values + dim * dim, sides[5].values + dim * 0); // Right -> Bottom
                        std::copy(temp_row, temp_row + dim, sides[2].values + dim * (dim - 1)); // Top -> Right
                        break;

                    case 2: // Back
                        // Rotate the top, left, bottom, and right sides
                        std::copy(sides[4].values + dim * 0, sides[4].values + dim, temp_row); // Top row of top
                        std::copy(sides[3].values + dim * (dim - 1), sides[3].values + dim * dim, sides[4].values + dim * 0); // Right -> Top
                        std::copy(sides[5].values + dim * (dim - 1), sides[5].values + dim * dim, sides[3].values + dim * (dim - 1)); // Bottom -> Right
                        std::copy(sides[1].values + dim * 0, sides[1].values + dim, sides[5].values + dim * (dim - 1)); // Left -> Bottom
                        std::copy(temp_row, temp_row + dim, sides[1].values + dim * 0); // Top -> Left
                        break;

                    case 1: // Left
                        // Rotate the top, front, bottom, and back sides
                        std::copy(sides[4].values + dim * 0, sides[4].values + dim, temp_row); // Top row of top
                        std::copy(sides[0].values + dim * 0, sides[0].values + dim, sides[4].values + dim * 0); // Front -> Top
                        std::copy(sides[5].values + dim * (dim - 1), sides[5].values + dim * dim, sides[0].values + dim * 0); // Bottom -> Front
                        std::copy(sides[2].values + dim * (dim - 1), sides[2].values + dim * dim, sides[5].values + dim * (dim - 1)); // Back -> Bottom
                        std::copy(temp_row, temp_row + dim, sides[2].values + dim * (dim - 1)); // Top -> Back
                        break;

                    case 3: // Right
                        // Rotate the top, front, bottom, and back sides
                        std::copy(sides[4].values + dim * 0, sides[4].values + dim, temp_row); // Top row of top
                        std::copy(sides[0].values + dim * (dim - 1), sides[0].values + dim * dim, sides[4].values + dim * 0); // Front -> Top
                        std::copy(sides[5].values + dim * 0, sides[5].values + dim, sides[0].values + dim * (dim - 1)); // Bottom -> Front
                        std::copy(sides[2].values + dim * 0, sides[2].values + dim, sides[5].values + dim * 0); // Back -> Bottom
                        std::copy(temp_row, temp_row + dim, sides[2].values + dim * 0); // Top -> Back
                        break;

                    case 4: // Top
                        // Rotate the front, left, back, and right sides
                        std::copy(sides[0].values + dim * 0, sides[0].values + dim, temp_row); // Front -> temp
                        std::copy(sides[1].values + dim * 0, sides[1].values + dim, sides[0].values + dim * 0); // Left -> Front
                        std::copy(sides[2].values + dim * 0, sides[2].values + dim, sides[1].values + dim * 0); // Back -> Left
                        std::copy(sides[3].values + dim * 0, sides[3].values + dim, sides[2].values + dim * 0); // Right -> Back
                        std::copy(temp_row, temp_row + dim, sides[3].values + dim * 0); // Front -> Right
                        break;

                    case 5: // Bottom
                        // Rotate the front, left, back, and right sides
                        std::copy(sides[0].values + dim * (dim - 1), sides[0].values + dim * dim, temp_row); // Front -> temp
                        std::copy(sides[3].values + dim * (dim - 1), sides[3].values + dim * dim, sides[0].values + dim * (dim - 1)); // Right -> Front
                        std::copy(sides[2].values + dim * (dim - 1), sides[2].values + dim * dim, sides[3].values + dim * (dim - 1)); // Back -> Right
                        std::copy(sides[1].values + dim * (dim - 1), sides[1].values + dim * dim, sides[2].values + dim * (dim - 1)); // Left -> Back
                        std::copy(temp_row, temp_row + dim, sides[1].values + dim * (dim - 1)); // Front -> Left
                        break;
                }
            } else {
                switch (face_index) {
                    case 0: // Front
                        // Rotate the top, left, bottom, and right sides
                        std::copy(sides[4].values + dim * (dim - 1), sides[4].values + dim * dim, temp_row); // Top row of top
                        std::copy(sides[2].values + dim * (dim - 1), sides[2].values + dim * dim, sides[4].values + dim * (dim - 1)); // Right -> Top
                        std::copy(sides[5].values + dim * (dim - 1), sides[5].values + dim * dim, sides[2].values + dim * (dim - 1)); // Bottom -> Right
                        std::copy(sides[1].values + dim * (dim - 1), sides[1].values + dim * dim, sides[5].values + dim * (dim - 1)); // Left -> Bottom
                        std::copy(temp_row, temp_row + dim, sides[1].values + dim * (dim - 1)); // Top -> Left
                        break;

                    case 2: // Back
                        // Rotate the top, left, bottom, and right sides
                        std::copy(sides[4].values + dim * 0, sides[4].values + dim, temp_row); // Top row of top
                        std::copy(sides[3].values + dim * (dim - 1), sides[3].values + dim * dim, sides[4].values + dim * 0); // Left -> Top
                        std::copy(sides[5].values + dim * 0, sides[5].values + dim, sides[3].values + dim * (dim - 1)); // Bottom -> Left
                        std::copy(sides[1].values + dim * (dim - 1), sides[1].values + dim * dim, sides[5].values + dim * 0); // Right -> Bottom
                        std::copy(temp_row, temp_row + dim, sides[1].values + dim * (dim - 1)); // Top -> Right
                        break;

                    case 1: // Left
                        // Rotate the top, front, bottom, and back sides
                        std::copy(sides[4].values + dim * 0, sides[4].values + dim, temp_row); // Top row of top
                        std::copy(sides[0].values + dim * (dim - 1), sides[0].values + dim * dim, sides[4].values + dim * 0); // Front -> Top
                        std::copy(sides[5].values + dim * 0, sides[5].values + dim, sides[0].values + dim * (dim - 1)); // Bottom -> Front
                        std::copy(sides[2].values + dim * (dim - 1), sides[2].values + dim * dim, sides[5].values + dim * 0); // Back -> Bottom
                        std::copy(temp_row, temp_row + dim, sides[2].values + dim * (dim - 1)); // Top -> Back
                        break;

                    case 3: // Right
                        // Rotate the top, front, bottom, and back sides
                        std::copy(sides[4].values + dim * 0, sides[4].values + dim, temp_row); // Top row of top
                        std::copy(sides[0].values + dim * 0, sides[0].values + dim, sides[4].values + dim * 0); // Front -> Top
                        std::copy(sides[5].values + dim * (dim - 1), sides[5].values + dim * dim, sides[0].values + dim * 0); // Bottom -> Front
                        std::copy(sides[2].values + dim * (dim - 1), sides[2].values + dim * dim, sides[5].values + dim * (dim - 1)); // Back -> Bottom
                        std::copy(temp_row, temp_row + dim, sides[2].values + dim * (dim - 1)); // Top -> Back
                        break;

                    case 4: // Top
                        // Rotate the front, left, back, and right sides
                        std::copy(sides[0].values + dim * 0, sides[0].values + dim, temp_row); // Front -> temp
                        std::copy(sides[3].values + dim * 0, sides[3].values + dim, sides[0].values + dim * 0); // Right -> Front
                        std::copy(sides[2].values + dim * 0, sides[2].values + dim, sides[3].values + dim * 0); // Back -> Right
                        std::copy(sides[1].values + dim * 0, sides[1].values + dim, sides[2].values + dim * 0); // Left -> Back
                        std::copy(temp_row, temp_row + dim, sides[1].values + dim * 0); // Front -> Left
                        break;

                    case 5: // Bottom
                        // Rotate the front, left, back, and right sides
                        std::copy(sides[0].values + dim * (dim - 1), sides[0].values + dim * dim, temp_row); // Front -> temp
                        std::copy(sides[3].values + dim * (dim - 1), sides[3].values + dim * dim, sides[0].values + dim * (dim - 1)); // Right -> Front
                        std::copy(sides[2].values + dim * (dim - 1), sides[2].values + dim * dim, sides[3].values + dim * (dim - 1)); // Back -> Right
                        std::copy(sides[1].values + dim * (dim - 1), sides[1].values + dim * dim, sides[2].values + dim * (dim - 1)); // Left -> Back
                        std::copy(temp_row, temp_row + dim, sides[1].values + dim * (dim - 1)); // Front -> Left
                        break;
                }
            }
        }

        void shuffle() {
            for (size_t i = 0; i < S; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    size_t rand_side = rand() % S;
                    size_t rand_index = rand() % N;
                    swap(this->sides[i].values[j], this->sides[rand_side].values[rand_index]);
                }
            }
        }

        int unsolved() {
            for (int i = 0; i < S; ++i) {
                for (int j = 0; j < N; ++j) {
                    if (this->sides[i].values[j] != this->sides[i].values[0]) return 1;
                }
            }
            return 0;
        }

        void print() {
            for (int i = 0; i < S; ++i) {
                cout << endl;
                cout << "Face " << i;
                for (int j = 0; j < N; ++j) {
                    if (!(j%this->dim)) cout << endl;
                    cout << this->sides[i].values[j] << " ";
                }
                cout << endl;
            }
        }

        void print_side() {
            for (int j = 0; j < N; ++j) {
                if (!(j%this->dim)) cout << endl;
                cout << this->sides[this->camera.face_index()].values[j] << " ";
            }
            cout << endl;
        }

        void rotate(char rotate_type) {
            switch(rotate_type) {
                case 'R':
                    this->camera.rotate_x(1);
                    break;
                case 'L':
                    this->camera.rotate_x(-1);
                    break;
                case 'U':
                    this->camera.rotate_y(1);
                    break;
                case 'D':
                    this->camera.rotate_y(-1);
                    break;
                default:
                    break;
            }
        }

        void turn(char turn_type) {
            int face_index = this->camera.face_index();
            if (turn_type == 'C') {
                cout << "Turning Clockwise" << endl;
                this->sides[face_index].rotate_clockwise();
                rotate_adjacent_sides(face_index, 1);

            } else if (turn_type == 'P') {
                cout << "Turning Counter Clockwise" << endl;
                this->sides[face_index].rotate_counterclockwise();
                rotate_adjacent_sides(face_index, 0);
            }
        }

        void executor() {
            cout << "Command(type help for reference sheet): ";
            safe_cin(this->input);

            if (this->input.input[0] == 'h') {
                cout << "The usage is SN, Where S is the direction, and N is the column/row number starting from the top left.\nRS can be used to Rotate, the side facing you where S is the direction.\nValid directions are: U, D, L, R.\nRW can be used to rotate. If W is C then clockwise, and if it is P then it's prime(counterclockwise)" << endl;
                cin.get();
            }
            if (this->input.input[0] == 'R') {
                rotate(this->input.input[1]);
            }
            if (this->input.input[0] == 'T') {
                turn(this->input.input[1]);
            }
        }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    
    const size_t sides = 6;
    const size_t area = 9;
    int side = 0;

    Cube<int, sides, area> cube;

    cube.init();
    cube.shuffle();

    while (cube.unsolved()) {
        cube.print();

        cout << "Current Side: " << endl;
        cube.print_side();
        cube.executor();
    }

    return 0;
}