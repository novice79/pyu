gh repo create pyu --public
git remote add origin n79:novice79/pyu
<!-- git remote set-url origin n79:novice79/pyu -->

lldb --file ./dist/bin/app

<!-- todo: rebuild all android lib with android-ndk-r25c -->
<!-- create empty branch -->
git switch --orphan file
git commit --allow-empty -m "Initial commit on orphan branch"
git push -u origin <new branch>