git stash
git config --global user.email "rmandrad@gmail.com"
git config --global user.name "rmandrad"
git checkout master
git remote set-url origin  https://github.com/openwrt/openwrt.git
git pull origin master --allow-unrelated-histories
git remote set-url origin https://github.com/rmandrad/openwrt.git
git push origin master 
