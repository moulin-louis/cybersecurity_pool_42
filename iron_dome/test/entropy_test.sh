#!/usr/bin/fish
while true;
  echo 'creating file'
  echo "Bonjour" > file_test
  sleep 4
  echo 'changing file'
  echo "fsdjklfdskj3k2j4lj23kl4j23;;.;;,;,;//../.34fggdfgjgdfklhjKLGJKSDJFKGJmv,cm,vmxcn" > file_test
  echo 'reseting file'
  sleep 4
  echo "Bonjour" > file_test
  echo 'deleting file'
  rm file_test
end
