## Compile `stress-ng` with `intel`

```bash
git clone https://github.com/ColinIanKing/stress-ng.git
cd stress-ng
module puge >/dev/null 2>&1
module load intel/2022a
make clean
CC=icc make
```