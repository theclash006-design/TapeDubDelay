# Tape Dub Delay (MVP)

Dubスタイルのテープディレイ・エフェクター(VST3/AU/Standalone)を、JUCE + CMakeで実装した
プロトタイプです。「Bionic Supa Delay」のマニュアルで説明されている**機能**（テープディレイ、
ステレオ処理、フィルター、テンポシンク、フィードバックループ内ルーティングなど）を参考に、
同じ考え方のDSPをゼロから書き直したものです。コード・GUIグラフィック・ブランド名は
一切コピーしておらず、あくまで「同種の効果が得られる仕組み」を再構築しています。

## 現在の実装スコープ（MVP）

実装済み:

- **Tape Delay**: ノート分割(1/2,1/4,1/8,1/16) + GROOVE(triplet⇄dotted) + TRIM(L/R差) + x2、
  FEEDBACK、DIRECT MIX、フィードバックループ内のHIGH CUT/LOW CUT、FREEZE、SMOOTH（ディレイタイム
  変化のスルーレート）、WOW/FLUTTER（テープ speed variation）、テンポシンク
- **Stereo**: BALANCE、WIDTH（mid/side）、PINGPONG、DRIFT（L/R非対称フィードバック）、
  AUTO PAN（ディレイタイムに同期したLFO）
- **Filter**: LP/HP/BP/Notchのマルチモードフィルター（TPT State Variable Filter）、
  テンポシンクLFO、**PREスイッチ**（フィードバックループ内 or ディレイ後、のルーティング切替）
- **Reverb**: Algorithm A（4コンブ+2オールパスの小規模ネットワーク、より共振的で
  "boingy"な質感）と Algorithm B（`juce::dsp::Reverb`ベースの滑らかな質感）を
  MIX ABでブレンド。WIDTH、MIX（dry/wet）、**PREスイッチ**（フィードバックループ内/
  ディレイ後）に対応
- **Pitch Shift**: 2グレイン・クロスフェード方式のグラニュラーピッチシフター。
  ALGO（A/B/C/D/X、B・Dは短めグレイン=小さいシフト/デチューン向き、A・Cは長め
  グレイン=大きめシフト向き、Xはローファイ）、SEMI（±24半音）、FINE（3乗カーブで
  中央付近を微調整、両端で±2オクターブ相当）、GRAIN、MIX、**PREスイッチ**に対応。
  PRE有効時はディレイのフィードバックループ内に挿入され、リピートのたびに
  ピッチがずれ続ける「ピッチエスカレーター」ダブエフェクトになる

- **カスタムGUI**: `GenericAudioProcessorEditor`を廃止し、独自LookAndFeel
  （ダーク+アンバーのハードウェア風テーマ、コード/画像とも本家からの流用なし）による
  タブ構成のエディタに変更。DELAY/STEREO/FILTER/PITCH/REVERBの5タブに、
  回転ノブ・ロッカースイッチ風トグル・ドロップダウンを自作コンポーネントとして配置

未実装（次のステップ、下記ロードマップ参照）:

- フィルターの残り8タイプ（LP4/HP2/HP4/BP、Peak/Notch/Shelf等のアルゴリズムB）
- フローダイアグラムGUI（本家にあるシグナルフロー可視化パネル）
- MIDI Learn（下記「MIDI操作について」参照）

## ビルド方法

必要なもの: CMake 3.15以上、C++コンパイラ（Windows: Visual Studio、Mac: Xcode、
Linux: GCC/Clang）、Git、インターネット接続（初回のみ、JUCE本体を自動取得します）。

```bash
cmake -B build
cmake --build build --config Release
```

初回ビルドはJUCE本体（~300MB）をGitHubから取得するため時間がかかります。
ビルド後、プラグインは以下に出力されます:

- `build/TapeDubDelay_artefacts/Release/VST3/Tape Dub Delay.vst3`
- `build/TapeDubDelay_artefacts/Release/AU/Tape Dub Delay.component` (Macのみ)
- `build/TapeDubDelay_artefacts/Release/Standalone/Tape Dub Delay.exe` (単体アプリとして即確認可能)

VST3フォルダにコピーしてDAWで読み込んでください（`COPY_PLUGIN_AFTER_BUILD`を有効にしているので
通常は自動でシステムのVST3フォルダにもコピーされます）。

## GUIについて

`Source/UI/`以下に自作のカスタムGUIがあります:

- `DubLookAndFeel.h/.cpp`: 配色・ノブ/トグル/タブの描画を定義するLookAndFeel
- `Controls.h`: ラベル+ウィジェット+APVTS attachmentをまとめたKnob/Toggle/Choice
- `SectionPanel.h`: タブ1枚分のコントロールをFlexBoxで折り返し配置するパネル
- `PluginEditor.cpp`: 上記を組み合わせてDELAY/STEREO/FILTER/PITCH/REVERBの
  5タブを構築

配色・レイアウトともゼロから作った独自デザインです（本家のノブ画像やパネル配置は
一切使っていません）。ウィンドウはリサイズ可能（620×420〜1100×800）。

本家にあるようなシグナルフローダイアグラム（VUメーター切替パネル）はまだ未実装です
（Roadmap参照）。

## MIDI操作について

本家の「MIDI Learn」機能は、VST3ホスト側の一般的なパラメータオートメーション/MIDIマッピング
機能でほぼ代替できます。JUCEのAPVTSで公開したパラメータは、Ableton Live、Bitwig、Cubase等の
ホストがVST3の「パラメータへのMIDI CCマッピング」機能を提供しているため、プラグイン側に
専用のLearn実装を追加しなくても近い体験が得られます。プラグイン内蔵のLearn UIが必要な場合は
`juce::AudioProcessorParameter`にMIDI CCを紐づける仕組みを別途追加してください。

## ロードマップ（フルスコープへの拡張）

1. **Filter拡張**: `ModFilter::Type`に`LowPass4`, `HighPass2/4`, `Peak`, `LowShelf`,
   `HighShelf`を追加し、アルゴリズムA/Bの切替パラメータを追加。
2. **Pitch Shiftのアルゴリズム差別化**: 現状ALGO A/B/C/Dはグレイン長以外ほぼ同じ
   エンジンを共有している。B/Dをより滑らかな位相補間（例: 4グレイン化）にする、
   A/Cにフォルマント補正を足す、などで本家によりニュアンスを近づけられる。
3. **信号ルーティングの一般化**: 現状はDelay→Stereo→Filter→PitchShift→Reverbの
   固定順（PREはフィードバックループ内挿入のみ選べる）。将来的に各セクションの
   `PRE`フラグと有効/無効を見て、実行順を動的に並べ替える小さな「シグナルチェイン
   組み立て」関数を書くと、本家の「フレキシブルルーティング」に近づく。
4. **フローダイアグラムパネル**: 現在の選択中ルーティングを絵で見せる本家のVU/
   フロー切替パネル相当のものを、タブとは別に上部に追加する。
5. **ノブのドラッグ感度・見た目の微調整**: 現状の`Knob`は標準の
   `RotaryHorizontalVerticalDrag`のまま。感度カーブや、値によって色が変わる
   インジケーターなど質感を上げる余地がある。

## 注意点・近似している部分

マニュアルには内部アルゴリズムの正確な数式は書かれていないため、以下は聴感でチューニングが
必要な近似実装です:

- TRIM: 単一ノブでL/Rのディレイタイムを対称に±50%まで開く実装にしています
- WOW/FLUTTERの深さ（±8ms/±3ms目安）は仮の値です
- AUTO PAN/Filter LFOの「x1/x2/:2/:4」は現在のディレイタイムに対する比率として実装しています
- Reverb Algorithm A（"boingy"）はコンブ4本+オールパス2段という小規模な自作ネットワークで、
  本家の内部アルゴリズムそのものではありません。Algorithm B（"natural/smooth"）は
  `juce::dsp::Reverb`（Freeverb系）をそのまま使っています
- Pitch Shiftは2グレイン・クロスフェード方式（delay-line modulation型グラニュラー
  シフター）の自作実装です。ALGO A/B/C/Dはグレイン長のプリセット差のみで、
  内部エンジンは共通。Xだけサンプル&ホールド+簡易量子化でローファイ感を追加しています

実際の質感に近づけるには、本家を耳で聴き比べながら上記のパラメータをチューニングしてください。
