#include <cstdio>
#include <cstdlib>

#include "gambatte.h"

#include "input.h"

int written_songs;
gambatte::GB gameboy;
Input input;
std::string out_path;

void run_one_frame() {
    size_t samples = 35112;
    long unsigned int audioBuffer[35112 + 2064];
    gameboy.runFor(0, 0, &audioBuffer[0], samples);
}

void wait(float seconds) {
    for (float i = 0.f; i < 60.f * seconds; ++i) {
        run_one_frame();
    }
}

void press(unsigned key, float seconds = 0.1f) {
    input.press(key);
    wait(seconds);
}

void load_song(int position) {
    press(SELECT);
    press(SELECT | UP);
    press(0);
    press(DOWN, 3);
    press(0);
    press(A);
    press(0);
    press(A);
    press(0);
    press(UP, 5); // scroll to top
    press(0);
    for (int i = 0; i < position; ++i) {
        press(DOWN);
        press(0);
    }
    // press song name
    press(A);
    press(0);
    // discard changes
    press(LEFT);
    press(0);
    press(A);
    // wait until song is loaded
    press(0, 5);
    if (gameboy.isSongEmpty()) {
        puts("OK");
        exit(0);
    }
    printf("Song %i...\n", ++written_songs);
}

bool sound_enabled;

void play_song() {
    sound_enabled = false;
    input.press(START);
    do {
        wait(1);
    } while(sound_enabled);
}

void on_ff_write(char, char) { }
void on_lcd_interrupt() { }

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: lsdrip <lsdj.gb>");
        return 1;
    }
    gameboy.setInputGetter(&input);
    gameboy.setWriteHandler(on_ff_write);
    gameboy.setLcdHandler(on_lcd_interrupt);
    gameboy.load(argv[1]);

    press(0, 3);

    int i = 0;
    while (true) {
        load_song(i++);
        play_song();
    }
}
