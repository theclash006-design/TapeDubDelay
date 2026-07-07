# Git セットアップ (Pacific Synthesis と同じ運用)

これまではzipで新しいコードを渡していましたが、Pacific Synthesisプロジェクトと同じように
Gitで管理すると、`git pull` だけで最新コードに更新できるようになります。

## 1. GitHubで空リポジトリを作る

github.com で新規リポジトリを作成（例: `TapeDubDelay`）。
**README/.gitignore/LICENSEは追加せず、完全に空の状態**で作成してください
（後述の手順でこちらから追加します）。

## 2. ローカルでGit初期化 (今の`TapeDubDelay`フォルダの中で)

`.gitignore`を`git add`より前に用意しておくのがポイントです。
Pacific Synthesisの初回pushでは、これを後回しにしたせいで`build-ios/`内の
100MB超のファイルがコミットに混ざり、GitHubにpushを拒否される事故がありました。
今回は`.gitignore`が最初から入っているので、その心配はありません。

```bash
cd ~/Downloads/TapeDubDelay

git init
git add -A
git status | head -20   # build/ が出ていないことを確認
git commit -m "Initial commit - Tape Dub Delay MVP"

# GitHubで作ったリポジトリのURLに差し替えてください
git remote add origin https://github.com/<あなたのアカウント>/TapeDubDelay.git
git branch -M main
git push -u origin main
```

`git status`の時点で`build/`が出ていれば`.gitignore`が効いていないので、
一度`git rm -r --cached build`してから`git add -A`をやり直してください。

## 3. 以後の更新の受け取り方

こちらでコードを直したら、変更点を教えるので、その都度:

```bash
git add -A
git commit -m "変更内容"
git push
```

という形で自分のGitHubに反映していく運用にできます（Pacific Synthesisと同じ流れです）。
複数マシン（Mac/Windowsなど）で作業する場合は、そちらでは:

```bash
git pull
```

だけで最新状態になります。

## 4. ビルドコマンド自体は変更なし

```bash
cmake -B build
cmake --build build --config Release
```

これはPacific Synthesisと既に同じです。今回Gitを導入するのは
「コードの受け渡し方法」を揃えるためで、ビルドコマンド自体はもともと共通でした。
