#include "one.h"

int main()
{
    one_seq_t sequence;
    uint32_t next = 0u;
    return one_seq_init_bits(&sequence, 8u) == ONE_SEQ_OK &&
                   one_seq_next(&sequence, 255u, &next) == ONE_SEQ_OK && next == 0u
               ? 0
               : 1;
}
