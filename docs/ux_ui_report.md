# Relatório de Análise UI/UX - Decent Sampler Editor

Este relatório mapeia as melhorias visuais e de usabilidade (UI/UX) levantadas na análise para os respectivos arquivos no código-fonte do projeto C++/Qt. O objetivo é fornecer um guia claro para o desenvolvedor aplicar o polimento necessário.

## 1. Abas da Dock (Dock Tabs)

> [!WARNING]
> **Problema:** O visual das abas é muito rústico e "blocado". Falta uma distinção clara entre abas ativas e inativas (apenas uma linha laranja não é suficiente), e o preenchimento (padding) poderia ser melhor.

**Onde alterar no código:**
- **Arquivo:** `src/ui/style.qss`
- **Linhas (aprox.):** 13 a 15 (`QTabBar::tab`)
- **Ação:**
  - Modifique `QTabBar::tab` para ter um fundo mais escuro (ex: `#1a1a1a`) para abas inativas.
  - Modifique `QTabBar::tab:selected` para compartilhar exatamente a mesma cor de fundo do painel principal (ex: `#1e1e1e`), criando uma fusão visual, e não apenas a borda superior laranja.
  - Aumente o `padding` para `8px 24px` para dar mais respiro.

**Nota sobre o nome "Mixer _FX":**
Verifiquei que no arquivo `src/ui/core/DockManager.cpp` (Linha 231), o texto já foi atualizado para `"Mixer & FX"`. Se na interface compilada ainda aparece `"Mixer _FX"`, verifique se há *cache* de compilação ou se a string foi sobrescrita em outra parte do código.

---

## 2. O Mixer e Modo Foco

### 2.2 Contraste das Legendas de Grupo
> [!IMPORTANT]
> **Problema:** Os textos verdes (ex: "Group 1") sobre o fundo cinza escuro na base dos canais do mixer têm legibilidade muito baixa.

**Onde alterar no código:**
- **Arquivo:** `src/ui/mixer/MixerChannelStrip.cpp` (ou no `.qss` aplicável)
- **Ação:** Identifique o `QLabel` que renderiza o nome do grupo na parte inferior do channel strip. Mude a cor do texto para uma cor com maior contraste (como `#d4d4d4` ou um verde bem mais vibrante e claro). 

### 2.3 Botões Redundantes (Exit Focus Mode)
> [!TIP]
> **Problema:** O texto/botão de saída do modo foco aparece duplicado na tela (no topo direito e na base direita).

**Onde alterar no código:**
- **Arquivo:** `src/ui/core/DockManager.cpp` e `src/ui/core/FocusModeOverlay.cpp`
- **Ação:** O `m_focusBtn` muda seu texto para `"⤢ Exit Focus Mode"` (DockManager.cpp:206). Se o `FocusModeOverlay` também instancia um botão de fechar (no canto superior direito), remova um deles para evitar poluição visual.

### 2.4 Faders e Knobs "Chapados"
> [!NOTE]
> **Problema:** Os gráficos dos faders e knobs carecem de profundidade, parecendo amadores.

**Onde alterar no código:**
- **Arquivos:** `src/ui/components/FaderWidget.cpp` e `src/ui/components/KnobWidget.cpp`
- **Ação:** Esses componentes carregam arquivos SVG (`fader_cap.svg`, `knob_indicator.svg`). A correção não é no código C++, mas sim na atualização desses arquivos vetoriais na pasta `src/ui/mixer/` ou `src/ui/assets/`. O designer deve adicionar gradientes, pequenos reflexos e sombras aos SVGs.

---

## 3. Página do Sintetizador (Synthesizer Page)

> [!WARNING]
> **Problema:** O painel "Synthesizer Parameters" está flutuando em um mar escuro, o que parece um layout não intencional ou inacabado.

**Onde alterar no código:**
- **Arquivo:** Possivelmente em um arquivo como `GroupEditorView.cpp` ou `SynthesizerView.cpp` (dependendo da sua estrutura, onde o dropdown "Waveform: Sawtooth" está inserido).
- **Ação:** Envolva esses controles de síntese em um "Card" (um `QWidget` ou `QGroupBox` com um *stylesheet* que defina um fundo sutilmente diferente, bordas arredondadas e padding). Use `QVBoxLayout` e `QHBoxLayout` combinados com `QSpacerItem` (stretch) para que este Card fique centralizado na tela, ou pelo menos ocupe uma área bem delimitada.

---

## 4. O Sequencer (Tabela)

> [!TIP]
> **Problema:** A tabela do sequenciador é crua. Não há diferenciação clara do cabeçalho e os botões de ação estão sem margem na parte inferior.

**Onde alterar no código:**
- **Arquivos:** `src/ui/style.qss` e `src/ui/sequencer/NoteSequenceEditorView.cpp`
- **Ação (`style.qss`):**
  - Adicione regras para `QHeaderView::section` para dar uma cor de fundo distinta, negrito na fonte ou uma borda inferior clara (algumas já existem, mas precisam de contraste).
  - Adicione uma regra `QTableWidget::item:hover { background: #333; }` para dar feedback ao passar o mouse.
- **Ação (`NoteSequenceEditorView.cpp`):**
  - No layout que contém os botões `+ Add Step`, `- Remove Step` e `Save Changes`, adicione margens com `layout->setContentsMargins(10, 10, 10, 10)` para desgrudá-los das bordas.

---
*Este relatório foi gerado para direcionar os ajustes necessários de UI/UX na base de código atual, elevando a percepção de qualidade do software.*
