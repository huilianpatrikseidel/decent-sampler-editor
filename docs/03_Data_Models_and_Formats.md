# Modelos de Dados e Transpilers

## 1. O Modelo Agnóstico Interno (The Custom State)
O sistema **não** salvará os projetos de trabalho no formato final (Decent Sampler ou SFZ). Tais formatos são motores de reprodução de áudio e não possuem campos para armazenar "estado da UI do editor", como posições X/Y de nós no gráfico de Node ou Histórico de Undo.
- **O Formato Proprietário (`.sampleride` / `.dsviz`)**: Baseado em JSON estruturado, guardará a árvore completa de estado da aplicação. 

## 2. Camada de Abstração: Os Transpilers
Os motores de compilação (Build Targets) lerão o modelo em memória JSON e converterão na sintaxe de áudio final escolhida pelo usuário.

### 2.1. Transpiler Decent Sampler
- Converte a estrutura de mapeamento, routing e ADSR em formato `<DecentSampler> ... </DecentSampler>`.
- Responsável por exportar o layout WYSIWYG do Canvas para a tag `<ui>`.
- **O Empacotador `.dsbundle` (Deploy Inteligente)**:
  - Processa uma varredura de todos os `.wav` utilizados.
  - Converte-os em segundo plano para `.flac` (via libflac) para compactação otimizada sem perdas de áudio.
  - Agrupa os FLACs, as imagens sprites (`multiFrameImage`) e o `.dspreset`.
  - Zipa tudo usando algoritmos de compressão nativos de C++, alterando a extensão para `.dsbundle`.

### 2.2. Transpiler SFZ
- Lê o mesmo modelo JSON e converte em texto (opcodes).
- Como o formato SFZ não possui interface gráfica WYSIWYG padronizada, ele ignora os elementos visuais de UI, mas transpila perfeitamente toda a lógica sonora:
  - `<region>` para samples (in, out, vel, keys).
  - `ampeg_a`, `ampeg_d`, `ampeg_s`, `ampeg_r` para os envelopes ADSR desenhados graficamente.
  - `seq_length` e `seq_position` para a lógica de Round Robins.
