# Sprints de Desenvolvimento (Plano de Execução AI)

O desenvolvimento será fragmentado nestes Sprints técnicos lineares. A IA deve assumir e validar rigorosamente o resultado de um Sprint antes de avançar.

## Sprint 1: Fundação do C++ e Build System
- Setup da pasta principal e repositório.
- Criação do `CMakeLists.txt` configurando C++17 e a Toolchain do `vcpkg`.
- Instalação e verificação das dependências (Qt6, RtAudio, libsndfile, GTest).
- Setup da arquitetura base (Application, MainWindow) e do projeto `Google Test` na pasta `/tests`.

## Sprint 2: Core e Gestão de Estado
- Implementar as árvores do modelo de dados em C++ (`ProjectManager`, `Node`, `SampleGroup`, `Zone`).
- Sistema de Serialização e Desserialização de JSON (`.sampleride`).
- Implementação inicial do `QUndoStack` (Command Pattern). 

## Sprint 3: Áudio Engine e Lock-Free
- Implementação da classe `AudioCallback` via `RtAudio`.
- Criação das filas lock-free (`concurrentqueue` ou `std::atomic`) comunicando o núcleo de dados com a thread de áudio.
- Leitura de arquivo wav (libsndfile) e envio de um teste básico de sinal (beep ou amostra) escutando portas MIDI.

## Sprint 4: O Canvas WYSIWYG e Componentes UI
- Criação das views em `QGraphicsScene/View` para o UI Designer.
- Classes customizadas que renderizam Sprites/Filmstrips a partir de imagens importadas.
- Property Inspector lateral e integração do Undo/Redo com movimentação no Canvas.

## Sprint 5: Mapeador e Waveform Gráfico
- O grid de Piano Roll 3D (Zonas de Mapping, KeySwitches, e Tabs para Round Robins/Múltiplos Mics).
- Waveform Editor com desenho via hardware (QPainter ou OpenGL), com as alças matemáticas de Loop, Crossfade e Pontos In/Out.
- Painel de gráfico vetorial ADSR acoplado aos modulares.

## Sprint 6: Node Graph (Roteamento de Buses e FX)
- View baseada em Nodes para desenhar Buses auxiliares e Efeitos (Delay, Reverb, Filtros).
- O Painel de Bindings listando os nós criados vs Botões da Interface.

## Sprint 7: Transpilers (TDD Driven) e Exportação `.dsbundle`
- Começando pela suíte GTest, garantir que conversores XML/SFZ sejam aprova de falhas.
- Rotina C++ que chama a compressão para `.flac`.
- ZIP empacotador (zlib ou minizip) gerando o `.dsbundle`.

> **Regra de Ouro da Execução**: A cada novo arquivo adicionado que envolva conversão de texto/xml, adicionar o respectivo caso de teste em `/tests`. Se envolver áudio, garantir que a manipulação de dados passe pela Queue lock-free.
