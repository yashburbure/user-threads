#!/bin/bash

make -C ./many-one 
make -C ./many-many
make compare
./many-one/main
./many-many/main
./compare
