# Arquitetura e Engenharia de Software (Sampler IDE)

## 1. Visão Geral da Arquitetura
A IDE de Sampler será construída utilizando **C++17** e o framework **Qt 6**. Ela adota o paradigma **MVC (Model-View-Controller)** com forte segregação entre a Interface Gráfica (Main Thread) e o Processamento de Áudio (Audio Callback Thread).

## 2. As 3 Leis de Engenharia de Áudio C++
Para garantir nível de produção comercial sem "glitches" (engasgos de áudio), adotaremos:

### 2.1. Thread Safety: Lock-Free Queues
A comunicação entre o painel do Qt e o Motor de Áudio não pode utilizar `std::mutex` bloqueantes, alocação de memória (`new`/`malloc`) ou I/O pesado dentro da Thread de Áudio.
- O motor de áudio consumirá comandos a partir de filas **Lock-Free (Ring Buffers)** estruturadas com variáveis `std::atomic`. Ex: Mexer no fader da UI empurra uma mensagem "SET_GAIN" para a fila, que é consumida silenciosamente pelo áudio no próximo ciclo de callback.

### 2.2. Gestão de Dependências (CMake + vcpkg)
Para viabilizar cross-compilation (Windows, Mac, Linux) sem complexidade local:
- Usaremos o **vcpkg** (da Microsoft) em modo manifesto (`vcpkg.json`) ou **FetchContent** nativo no `CMakeLists.txt`.
- Dependências geridas automaticamente: `Qt6`, `RtAudio` (ou miniaudio), `libsndfile`, `libflac`, `concurrentqueue` (se necessário).

### 2.3. Test-Driven Development (TDD)
O núcleo lógico do software (Transpilers e Parsers XML/SFZ) deve ser coberto por testes para evitar falhas silenciosas na compilação dos presets.
- Utilizaremos o **Google Test (GTest)** incorporado via CMake na pasta `/tests`.

## 3. Gestão de Estado (Undo/Redo)
Softwares WYSIWYG baseados em drag & drop não perdoam erros. 
- Implementação do Padrão **Command Pattern**. Toda ação (Mover Sample, Ligar Cabo, Alterar ADSR) herda de `QUndoCommand`.
- O histórico global é mantido pelo `QUndoStack` nativo do Qt, garantindo que atalhos como `Ctrl+Z` e `Ctrl+Y` sejam onipresentes e seguros.
