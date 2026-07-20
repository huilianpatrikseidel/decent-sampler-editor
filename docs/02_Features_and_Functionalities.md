# Funcionalidades e Regras de Negócio

## 1. Auto-Mapping Inteligente (Baseado em Regex/Tokens)
Para viabilizar a criação de instrumentos massivos (orquestrais), o software suporta importação em lote com detecção automática de metadados via nome do arquivo.
- O usuário define um token (ex: `{Nota}_{Mic}_{Velocity}_{RR}.wav`).
- Ao arrastar 800 amostras (ex: `C3_Close_127_RR2.wav`), a IDE preenche instantaneamente o grid 3D, distribuindo notas, abas de microfone, camadas de velocidade e empilhando os Round Robins sem intervenção manual.

## 2. Motor de Auditioning, MIDI e Debugger em Tempo Real
A IDE não é "cega" nem "surda".
- **Backend de Áudio**: Usará `RtAudio` para suportar ASIO no Windows, CoreAudio no Mac e ALSA/JACK no Linux, com latência ultra-baixa.
- **Teclado Virtual & Monitor MIDI**: Um teclado na base da tela para audição. Acoplado a ele, um painel de Console/Log de diagnóstico listando todos os eventos MIDI que entram (Note On, CC, Pitch Bend), permitindo debugar facilmente transições de camadas e Key Switches.

## 3. Editor WYSIWYG e Gerenciador Global (Metadados)
- **Canvas (QGraphicsView)**: Arraste e solte de componentes de interface, suportando Filmstrips (Sprites) para simulação de rotação visual.
- **Project Settings & Global UI**: Aba dedicada para Nome do Instrumento, Autor, Versão, EULA e Background. Estes dados são injetados diretamente nos headers do XML ou SFZ pelo Transpiler.

## 4. Roteador de Áudio e Efeitos (Node-Based)
Fluxo de sinal desenhado visualmente:
- Criação de **Buses (Auxiliares)** para submixagens.
- Cabos virtuais ligando um `Grupo de Samples` a um `Delay`, e deste para o `Master Bus`.

## 5. Waveform, Sample Editor e Mapeamento 3D
- Renderização visual das amostras `.wav` com marcadores arrastáveis (`In`, `Out`, `Loop Start/End` e curva visual de `Crossfade`).
- **Round Robins e Key Switches**: Uso de `seqPosition` empilhado e marcação de teclas de mudança de articulação fora da zona tocável.
- **Múltiplos Microfones**: Sistema de abas para gerenciar gravação multi-pista (Close, Overhead, Room).

## 6. Moduladores e Macros (Multi-Bindings)
- **ADSR Vetorial e LFOs**: Gráficos de envelope de amplitude manipuláveis via nós. 
- **Macros Inteligentes**: O usuário pode criar botões "Macro" invisíveis ou visíveis que controlam múltiplos parâmetros simultaneamente (ex: um botão "Morph" que sobe cutoff e desce tempo de delay ao mesmo tempo usando curvas matemáticas customizadas).

## 7. DSP e Exportação em Lote (Batch Preset Generation)
- **DSP e Normalização**: Auto-gain por metadados (Não-destrutiva) e Processamento Batch (Destrutiva).
- **Exportação Multi-Target**: O Transpiler permite gerar versões "PRO" e "LITE" do instrumento a partir do mesmo arquivo `.sampleride`, ativando ou desativando camadas de microfones automaticamente na geração dos `.dsbundle` ou `.sfz`.
