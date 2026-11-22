#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

const int BITS = 8;              // board = 8 lights = 1 byte
const unsigned int MASK = 0xFF;  // 11111111 = keeps only lower 8 bits

int lives = 3;   // total chances
int score = 0;   // accumulated points

// Reverses bit order. Loop: extract bit i, place at position (7 - i).
// Example: 11010000 -> 00001011
unsigned int reverseBits(unsigned int x) {
    unsigned int mirroredNumber = 0;
    for (int bitIndex = 0; bitIndex < BITS; bitIndex++) {
        unsigned int bit = (x >> bitIndex) & 1;          // isolate bit i
        int oppositePosition = BITS - 1 - bitIndex;       // mirror position
        mirroredNumber = mirroredNumber | (bit << oppositePosition);
    }
    return mirroredNumber;
}

// Circular left shift by n.
// x << n moves bits left (overflow above bit 7).
// x >> (8 - n) brings the overflowed bits back to the right.
// OR combines both. & MASK clears any bits above bit 7.
unsigned int rotateLeft(unsigned int x, int n) {
    unsigned int bitsShiftedLeft = x << n;
    unsigned int bitsWrappedAround = x >> (BITS - n);
    return (bitsShiftedLeft | bitsWrappedAround) & MASK;
}

// ~x flips ALL 32 bits (leading zeros become 1s too).
// & MASK trims result back to 8 bits -> true 8-bit complement.
unsigned int invert(unsigned int x) {
    return (~x) & MASK;
}

// Counts set bits (green lights). Loop checks each bit with & 1.
int countBits(unsigned int x) {
    int count = 0;
    for (int bitIndex = 0; bitIndex < BITS; bitIndex++) {
        count = count + ((x >> bitIndex) & 1);
    }
    return count;
}

// Caesar cipher on uppercase A-Z only.
// %26 because alphabet has 26 letters -> wraps Z back to A.
// (currentChar - 'A') maps A..Z to 0..25, add shift, mod 26, add 'A' back.
string shiftSecretMessage(string message, int shift) {
    for (int i = 0; i < (int)message.size(); i++) {
        char currentChar = message[i];
        if (currentChar >= 'A' && currentChar <= 'Z') {
            message[i] = 'A' + (currentChar - 'A' + shift) % 26;
        }
    }
    return message;
}

// Converts number to 8-char binary string, Most Significant Bit first
// (bit 7 down to bit 0).
string convertToBinary(unsigned int x) {
    string s = "";
    for (int bitIndex = BITS - 1; bitIndex >= 0; bitIndex--) {
        int isBitSet = (x >> bitIndex) & 1;
        if (isBitSet) s = s + "1";
        else s = s + "0";
    }
    return s;
}

// Normalizes input: strips spaces, tabs, carriage returns, uppercases letters.
// Purpose: so "green" == "GREEN" == " green " match the same answer.
string clean(string rawInput) {
    string result = "";
    for (int i = 0; i < (int)rawInput.size(); i++) {
        char c = rawInput[i];
        if (c == ' ' || c == '\t' || c == '\r') continue;
        if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
        result = result + c;
    }
    return result;
}

// Prints rulebook. Called when user types "?".
void guide() {
    cout << endl << "Guidelines" << endl;
    cout << "MIRROR (Reverse the Bits Order)" << endl;
    cout << "ROLL (Rotate the bits towards the left, and place the bits that fall off at the right end)" << endl;
    cout << "COUNT (Check for the 1 bits (Green))" << endl;
    cout << "INVERT (Change 1s to 0s and 0s to 1s)" << endl;
    cout << "XOR (Same Bits = 0 , Different Bits = 1)" << endl;
    cout << "CAESAR (Go back in letters to make the word meaningful)" << endl;
    cout << "EXIT (Type q to quit the game)" << endl;
}

// Input handler for every question.
// getline reads whole line -> clean() normalizes it.
// "?" -> guide, "Q" -> quit, "" -> re-prompt.
// Wrong answer -> lives--, if lives==0 -> game over.
// Returns nothing; exits loop only on correct answer.
void ask(string question, string correctAnswer) {
    while (true) {
        cout << question << endl << "> ";
        string line;
        if (!getline(cin, line)) exit(0);   // handles End Of File
        string answer = clean(line);

        if (answer == "?") { guide(); continue; }
        if (answer == "Q") { cout << "Game Over." << endl; exit(0); }
        if (answer == "") continue;

        if (answer == clean(correctAnswer)) return;  // correct -> exit

        lives--;
        cout << "Wrong Answer. " << "Remaining Lives: " << lives << endl << endl;
        if (lives == 0) {
            cout << endl << "YOU ARE ELIMINATED. Final score: " << score << endl;
            exit(0);
        }
    }
}

// Adds points to score, prints confirmation.
void win(int points) {
    score = score + points;
    cout << " +" << points << " points!" << endl;
}

// Intro text only.
void intro() {
    cout << endl << "ESCAPE THE SQUID GAME" << endl << endl;
    cout << "Clear the 5 rounds to win the grand prize" << endl;
    cout << "Each round has a Board of 8 lights:" << endl;
    cout << "Green (1) or Red (0). You have 3 chances in total." << endl;
    cout << "Type ? at any question to read the rulebook." << endl;
}

// Round 1: check Most Significant Bit only.
// (board >> 7) & 1 gives leftmost bit. 1 -> "G", 0 -> "R".
void round1() {
    unsigned int board = rand() % 256;

    cout << endl << "ROUND 1: RED LIGHT, GREEN LIGHT";
    cout << " (Chances: " << lives << ", Score: " << score << ")" << endl << endl;
    cout << "Board: " << convertToBinary(board) << endl;
    cout << "Only the light at the very front (leftmost) decides who lives." << endl;

    string frontAnswer = "R";
    if ((board >> (BITS - 1)) & 1) frontAnswer = "G";
    ask("Is the front light Green or Red? Type G or R:", frontAnswer);

    cout << "You cross the line.";
    win(100);
}

// Round 2: reverse bits. do-while ensures mirrored != original
// so the question is never trivial.
void round2() {
    unsigned int board, mirrored;
    do {
        board = rand() % 256;
        mirrored = reverseBits(board);
    } while (mirrored == board);

    cout << endl << "ROUND 2: TUG OF WAR";
    cout << " (Chances: " << lives << ", Score: " << score << ")" << endl << endl;
    cout << "Board: " << convertToBinary(board) << endl;
    cout << "The teams swap sides: front becomes back, back becomes front." << endl;

    ask("Type the Board as the other team sees it (8 digits):", convertToBinary(mirrored));

    cout << "Your team holds the rope.";
    win(100);
}

// Round 3: rotate left by random amount 2..6.
// do-while ensures rotation actually changes the board.
void round3() {
    int doorCode = 2 + rand() % 5;
    unsigned int board, rotated;
    do {
        board = rand() % 256;
        rotated = rotateLeft(board, doorCode);
    } while (rotated == board);

    cout << endl << "ROUND 3: MARBLES";
    cout << " (Chances: " << lives << ", Score: " << score << ")" << endl << endl;
    cout << "Board: " << convertToBinary(board) << endl;
    cout << "A guard whispers: rotate the Board LEFT by " << doorCode << " places." << endl;

    ask("Rotated Board (8 digits):", convertToBinary(rotated));

    cout << "You keep your marbles. The door opens.";
    win(100);
}

// Round 4: two-step puzzle.
// Board constrained to 3..5 set bits so shift is small and word stays readable.
// Step 1: count green bits. Step 2: decode Caesar (shift letters BACK).
void round4() {
    unsigned int board;
    do {
        board = rand() % 256;
    } while (countBits(board) < 3 || countBits(board) > 5);
    int greenCount = countBits(board);

    string possibleSecretWords[3] = {"RED LIGHT", "GREEN LIGHT", "FRONT MAN"};
    string originalSecretWord = possibleSecretWords[rand() % 3];
    string encryptedSecretWord = shiftSecretMessage(originalSecretWord, greenCount);

    cout << endl << "ROUND 4: GLASS BRIDGE";
    cout << " (Chances: " << lives << ", Score: " << score << ")" << endl << endl;
    cout << "Board: " << convertToBinary(board) << endl;
    cout << "A note is taped to the glass. Every letter was shifted forward" << endl;
    cout << "by the number of green lights:" << endl;
    cout << encryptedSecretWord << endl;

    ask("Step 1 - How many lights are green?", to_string(greenCount));
    cout << "The glass holds. Now shift each letter BACK by " << greenCount << "." << endl;
    ask("Step 2 - Decoded note:", originalSecretWord);

    cout << "You cross the Glass Bridge.";
    win(100);
}

// Round 5: multi-step reverse.
// Forward: invert -> rotateLeft 2 -> XOR with key.
// Reverse order: XOR with key -> rotateRight 2 (= rotateLeft 6) -> invert.
// key excluded from {0, 0xFF} so XOR actually modifies bits.
void round5() {
    unsigned int board = rand() % 256;
    unsigned int key;
    do {
        key = rand() % 256;
    } while (key == 0 || key == MASK);
    unsigned int locked = rotateLeft(invert(board), 2) ^ key;

    cout << endl << "ROUND 5: THE SQUID GAME (FINAL)";
    cout << " (Chances: " << lives << ", Score: " << score << ")" << endl << endl;
    cout << "The Front Man drops his Key: " << convertToBinary(key) << endl;
    cout << "He locked the Board in three steps:" << endl;
    cout << "1) INVERT every light" << endl;
    cout << "2) ROTATE LEFT by 2" << endl;
    cout << "3) XOR with the Key" << endl;
    cout << "Locked Board: " << convertToBinary(locked) << endl;
    cout << "Undo the steps in REVERSE order to get the original Board." << endl;

    ask("Original Board (8 digits):", convertToBinary(board));

    cout << "The Front Man lowers his mask. The final door opens!";
    win(200);
}

// Bonus = remaining lives * 25. Rank depends on lives left.
void victory() {
    int bonus = lives * 25;
    score = score + bonus;
    string rank;
    if (lives == 3) { rank = "THE WINNER"; }
    else if (lives == 2) { rank = "FINALIST"; }
    else { rank = "SURVIVOR"; }
    cout << endl << "YOU SURVIVED THE SQUID GAME!" << endl;
    cout << "Bonus for chances left: +" << bonus << endl;
    cout << "FINAL SCORE: " << score << " RANK: " << rank << endl;
}

// Seeds random number generator, runs all rounds sequentially.
int main() {
    srand(time(0));
    intro();
    round1();
    round2();
    round3();
    round4();
    round5();
    victory();
    return 0;
}