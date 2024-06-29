
# Self Learning 3d Cars

## Online Demo Link

**`/!\ important /!\`**

http://guillaumebouchetepitech.github.io/self-learning-3d-cars/dist/index.html

**`/!\ important /!\`**

## Diagrams


### Main Logic

```mermaid

%%{
  init: {
    'theme': 'base',
    'themeVariables': {
      'primaryColor': '#242424',
      'primaryTextColor': '#DDD',
      'primaryBorderColor': '#000',
      'lineColor': '#A0A0A0',
      'secondaryColor': '#454545',
      'tertiaryColor': '#353535'
    }
  }
}%%

  flowchart BT

    subgraph simulation [Run The Simulation]

      direction LR

      run1["
        set the agents

        genomes
        artificial neural networks
        car agents logic
        physic vehicles
      "]
      run2["
        run the simulation
        until all genomes
        are done and rated
      "]

      run1 --> run2

    end

    subgraph evolution [Evolve The Simulation]

      direction LR

      evo1["
        Evolving
        (Natural Selection)
      "]
      evo2["
        Elitism -> 10%

        Select and keep some
        of the best genomes
      "]
      evo3["
        Reproduction and
        Mutation -> 80%

        cross breed
        the best genomes
        mutate the newly
        bred genomes
      "]
      evo4["
        Diversity -> anything left

        fill the
        generation with
        random genomes
      "]

      evo1 --> evo2 --> evo3 --> evo4

    end

    simulation -- evolve --> evolution
    evolution -. repeat .-> simulation


```

### Relationships

```mermaid

%%{
  init: {
    'theme': 'base',
    'themeVariables': {
      'primaryColor': '#242424',
      'primaryTextColor': '#DDD',
      'primaryBorderColor': '#000',
      'lineColor': '#A0A0A0',
      'secondaryColor': '#454545',
      'tertiaryColor': '#353535'
    }
  }
}%%

  erDiagram

    SIMULATION ||--|| GENETIC-ALGORITHM : manage
    SIMULATION ||--|{ CAR : manage
    GENETIC-ALGORITHM ||--|{ GENOME : manage
    GENETIC-ALGORITHM ||--|{ NEURAL-NETWORK : manage
    GENOME ||--|| NEURAL-NETWORK : use
    CAR ||--|| NEURAL-NETWORK : use

```

### WebAssembly Webworker version

```mermaid

%%{
  init: {
    'theme': 'base',
    'themeVariables': {
      'primaryColor': '#242424',
      'primaryTextColor': '#DDD',
      'primaryBorderColor': '#000',
      'lineColor': '#A0A0A0',
      'secondaryColor': '#454545',
      'tertiaryColor': '#353535'
    }
  }
}%%

  flowchart TB


    main["
      MAIN WASM

      setup worker

      request new simulation frames to the workers

      render an interpolated simulation frame
    "]

    worker["
      WORKER WASM

      reset simulation

      process simulation and generate a frame

      add X new car agent(s) to the simulation
    "]

    main <--> worker

```

### Mutilthreaded Producer Consumer Pattern

```mermaid

%%{
  init: {
    'theme': 'base',
    'themeVariables': {
      'primaryColor': '#242424',
      'primaryTextColor': '#DDD',
      'primaryBorderColor': '#000',
      'lineColor': '#A0A0A0',
      'secondaryColor': '#454545',
      'tertiaryColor': '#353535'
    }
  }
}%%

  sequenceDiagram

    autonumber

    participant MT as Main Thread
    participant P as Producer
    participant PT as Producer Thread
    participant CT as Consumer Thread


    rect rgb(32, 32, 32)

      rect rgb(64, 64, 64)

        MT->>+P: push task(s) to run in parallel

        rect rgb(128, 64, 64)

          Note over P: Thread Safe On
          P->>P: store new task
          P->>+PT: notify
          Note over P: Thread Safe Off

        end

      end

      rect rgb(128, 64, 64)

        Note over PT: Thread Safe On

        PT->>PT: wake up

        loop Until no more task to assign

          PT->>PT: check for available task(s)

          critical no more tasks

            PT-->>P: all tasks completed

            P-->>-MT: all tasks completed

          option a task was found

            PT->>PT: check for available consumer(s)


            break a consumer is available

              PT->>CT: assign task to run
              PT->>+CT: notify

            end

          end

        end

        PT->>-PT: sleep

        Note over PT: Thread Safe Off

      end

      rect rgb(64, 64, 64)

        CT->>CT: wake up
        CT->>CT: run task

        rect rgb(128, 64, 64)

          Note over CT: Thread Safe On
          CT->>CT: task completed
          CT-->>PT: set task as completed
          CT-->>PT: notify
          CT->>-CT: sleep
          Note over CT: Thread Safe Off

        end

      end


    end

```

# Dependencies

## Dependency: Emscripten 3.1.44 (for web-wasm build)
```bash
git clone https://github.com/emscripten-core/emsdk.git

cd emsdk

./emsdk install 3.1.44
./emsdk activate --embedded 3.1.44

. ./emsdk_env.sh

em++ --clear-cache
```

## Dependency: SDL2, GLESv2 (for native build)
```
libsdl2-dev
libglesv2
```

## Dependency: Geronimo 0.0.16

[Github Link](https://github.com/GuillaumeBouchetEpitech/geronimo)

This dependency will be downloaded and built with the `Build Everything` method below

## Dependency: Basic Genetic Algorithm 0.0.7

[Github Link](https://github.com/GuillaumeBouchetEpitech/basic-genetic-algorithm)

This dependency will be downloaded and built with the `Build Everything` method below

# How to Build

## Build Everything (will skip web-wasm if emscripten is absent)

```bash
sh sh_everything.sh
# will tell if a dependency is missing
# will skip the web-wasm build if emscripten is not detected
```

# How to Run

## Native Build

```
./bin/exec
```

## Web Wasm Build - without multithreading support: webworkers (mobile friendly)

```bash
node dumbFileServer.js # launch the file server
```

then use **firefox/chrome browser (should support desktop and mobile)** to load `http://127.0.0.1:9000/dist/index.html`

## Web Wasm Build - with multithreading support (desktop friendly)

```bash
node dumbFileServer.js # launch the file server
```

then use **firefox/chrome browser (desktop advisable)** to load `http://127.0.0.1:9001/dist/index.html`


# Thanks for watching!
