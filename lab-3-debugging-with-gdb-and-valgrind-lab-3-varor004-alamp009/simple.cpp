

int main() {
    int *p = new int[10];
    p[9] = 1;
    delete[] p;

    return 0;
}