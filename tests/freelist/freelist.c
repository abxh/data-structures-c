/*
    Freelist struct should be opaque if the library is made non-header-only. So freelist's struct member are not tested.

    Mutating operation types:
    - init
    - deallocate_all
    - deallocate
    - allocate
    - reallocate

    Branches:
    - allocate()
        |
        |
    - deallocate()
        |
        |
    - reallocate()
        |
        |
*/


int main() {

}
