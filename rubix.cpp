#include <iostream>
#include <limits>
#include <ios>
#include <cstdlib>
#include <ctime>


using namespace std;

template<typename T, size_t N> struct Input { T input[N] = {}; };

template<typename T, size_t N>
struct Side {
    T values[N];
};

template<typename T, size_t N, size_t S> 
class Cube { 
    unsigned int side;
    Side<T, N> sides[S];
    
    public:
        Cube() {
            this->side = 0;
        }

        void init() {
            for (size_t i = 0; i < S; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    this->sides[i].values[j] = static_cast<T>(i);
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
                    if (!(j%3)) cout << endl;
                    cout << this->sides[i].values[j] << " ";
                }
                cout << endl;
            }
        }

        void print_side() {
            for (int j = 0; j < N; ++j) {
                if (!(j%3)) cout << endl;
                cout << sides[side].values[j] << " ";
            }
            cout << endl;
        }

        void turn(char turn_type) {
            switch(turn_type) {
                case 'R':
                    ++this->side;
                    if (this->side > 5) {
                        this->side = 0;
                    }
                    cout << "Side: " << this->side << endl;

                    break;
                default:
                    this->side = this->side ? --this->side : 5;
                    cout << "Side: " << this->side << endl;
                    break;
            }
        }

        template<typename I, size_t I_N>
        void executor(Input<I, I_N>& input) {
            if (input.input[0] == 'h') {
                cout << "The usage is SN, Where S is the direction, and N is the column/row number starting from the top left.\nTS can be used to rotate the side facing you where S is the direction.\nValid directions are: U, D, L, R." << endl;
                cin.get();
            }
            if (input.input[0] == 'T') {
                turn(input.input[1]);
            }
        }
};

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

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    
    const size_t sides = 6;
    const size_t area = 9;
    int side = 0;

    Cube<int, area, sides> cube;
    Input<char, 3> input;

    cube.init();
    cube.shuffle();

    while (cube.unsolved()) {
        cube.print();

        cout << "Current Side: " << endl;
        cube.print_side();
        
        cout << "Command(type h for reference sheet): ";
        
        safe_cin(input);

        cube.executor(input);
    }

    return 0;
}