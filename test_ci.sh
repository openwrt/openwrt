BRANCH="main"
RET=0
for commit in $(git rev-list HEAD ^$BRANCH); do
  echo "=== Checking commit '$commit'"
  if git show --format='%P' -s $commit | grep -qF ' '; then
    echo "Pull request should not include merge commits"
    RET=1
  fi

  author="$(git show -s --format=%aN $commit)"
  if echo "$author" | grep -qE '\S\+\s\+\S\+|\[bot\]'; then
    echo "Author name ($author) seems ok"
  else
    echo "Author name ($author) need to be your real name 'firstname lastname'"
    RET=1
  fi

  subject="$(git show -s --format=%s $commit)"
  if echo "$subject" | grep -q -e '^[0-9A-Za-z,+/_\.-]\+: ' -e '^Revert '; then
    echo "Commit subject line seems ok ($subject)"
  else
    echo "Commit subject line MUST start with '<area>: ' ($subject)"
    RET=1
  fi

  body="$(git show -s --format=%b $commit)"
  sob="$(git show -s --format='Signed-off-by: %aN <%aE>' $commit)"
  if echo "$body" | grep -qF "$sob"; then
    echo "Signed-off-by match author"
  else
    echo "Signed-off-by is missing or doesn't match author (should be '$sob')"
    RET=1
  fi

  if echo "$body" | grep -v "Signed-off-by:"; then
    echo "A commit message exists"
  else
    echo "Missing commit message. Please describe your changes"
    RET=1
  fi
done

echo $RET
