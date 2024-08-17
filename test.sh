#! /bin/bash
ls ../testcases -1 | xargs -I {} ./a.out ../testcases/{}
