import std.stdio;
import std.string : chomp;
import std.array : replicate;

int value(char c) { return c == ' ' ? 0 : c - 'A' + 1; }
string shift(int from, int to, int modulus, char plus, char minus) {
    int forward = (to - from + modulus) % modulus;
    int backward = (from - to + modulus) % modulus;
    return forward <= backward ? replicate([plus], forward) : replicate([minus], backward);
}

struct Machine {
    int[30] stones;
    int pos;
    string emit(char c) {
        int target = value(c), best = int.max, chosen;
        string result;
        foreach (int zone; 0 .. 30) {
            auto code = shift(pos, zone, 30, '>', '<') ~ shift(stones[zone], target, 27, '+', '-') ~ ".";
            if (code.length < best) {
                best = cast(int)code.length;
                chosen = zone;
                result = code;
            }
        }
        pos = chosen;
        stones[pos] = target;
        return result;
    }
    string repeated(string block, int count) {
        int origin = pos;
        string code = shift(stones[pos], count, 27, '+', '-');
        stones[pos] = count;
        foreach (i, c; block) {
            pos = (origin + cast(int)i + 1) % 30;
            code ~= ">" ~ shift(stones[pos], value(c), 27, '+', '-');
            stones[pos] = value(c);
        }
        code ~= replicate("<", block.length) ~ "[";
        foreach (c; block) code ~= ">.";
        code ~= replicate("<", block.length) ~ "-]";
        pos = origin;
        stones[pos] = 0;
        return code;
    }
}

void main() {
    string phrase = stdin.readln().chomp();
    Machine state;
    string answer;
    size_t at;
    while (at < phrase.length) {
        Machine next = state;
        string chosen = next.emit(phrase[at]);
        size_t consumed = 1;
        long bestSaving;
        foreach (size_t width; 1 .. 15) {
            if (at + 2 * width > phrase.length) break;
            auto block = phrase[at .. at + width];
            int repeats = 1;
            while (repeats < 26 && at + (repeats + 1) * width <= phrase.length &&
                   phrase[at + repeats * width .. at + (repeats + 1) * width] == block) ++repeats;
            if (repeats < 2) continue;
            Machine plain = state;
            size_t plainCost;
            foreach (c; phrase[at .. at + repeats * width]) plainCost += plain.emit(c).length;
            Machine loopState = state;
            auto loopCode = loopState.repeated(block, repeats);
            long saving = cast(long)plainCost - cast(long)loopCode.length;
            if (saving > bestSaving) {
                bestSaving = saving;
                chosen = loopCode;
                next = loopState;
                consumed = repeats * width;
            }
        }
        answer ~= chosen;
        state = next;
        at += consumed;
    }
    writeln(answer);
}
