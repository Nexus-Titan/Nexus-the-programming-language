#include <stdio.h>
#include <string.h>
void compress_rle(const char* input) {
    int n = strlen(input);
    for (int i = 0; i < n; i++) {
        int count = 1;
        while (i < n - 1 && input[i] == input[i + 1]) {
            count++;
            i++;
        }
        printf("%c%d", input[i], count);
    }
}
class PhysicsBody {
public:
    float x, y, mass;
    void applyForce(float fx, float fy) {
        x += fx / mass;
        y += fy / mass;
    }
};
