#!/usr/bin/env bash

BRANCH="${BRANCH:-master}"

echo_red()   { printf "\033[1;31m$*\033[m\n"; }
echo_green() { printf "\033[1;32m$*\033[m\n"; }
echo_blue()  { printf "\033[1;34m$*\033[m\n"; }

RET=0
for commit in $(git rev-list HEAD ^origin/$BRANCH); do
  echo_blue "=== Checking commit '$commit'"
  if git show --format='%P' -s $commit | grep -qF ' '; then
    echo_red "Pull request should not include merge commits"
    RET=1
  fi

  author="$(git show -s --format=%aN $commit)"
  if echo $author | grep -q '\S\+\s\+\S\+'; then
    echo_green "Author name ($author) seems ok"
  else
    echo_red "Author name ($author) need to be your real name 'firstname lastname'"
    RET=1
  fi

  subject="$(git show -s --format=%s $commit)"
  if echo "$subject" | grep -q -e '^[0-9A-Za-z,+/_-]\+: ' -e '^Revert '; then
    echo_green "Commit subject line seems ok ($subject)"
  else
    echo_red "Commit subject line MUST start with '<area>: ' ($subject)"
    RET=1
  fi

  body="$(git show -s --format=%b $commit)"
  sob="$(git show -s --format='Signed-off-by: %aN <%aE>' $commit)"
  if echo "$body" | grep -qF "$sob"; then
    echo_green "Signed-off-by match author"
  else
    echo_red "Signed-off-by is missing or doesn't match author (should be '$sob')"
    RET=1
  fi
done

exit $RET
