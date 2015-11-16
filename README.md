# EssenGeTUMmelBot

This Telegram Bot helps you and your friend to plan attendances of meals.

## Build instructions

Install the required packages:

    sudo apt-get install g++ make binutils cmake libssl-dev libboost-system-dev libboost-iostreams-dev libboost-test-dev

To compile:

    git submodule update --init
    mkdir build/
    cd build/
    cmake ..
    make -j


## License
Copyright 2015 Florian Scheibner

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
