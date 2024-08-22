int main() {
    int *p = new int(10);

    delete p;
    delete p;

    return 0;
}
