while true
  sleep 1.5
  hid_task("\x04\000\000\000\000\000")
  sleep 0.01
  hid_task("")
  sleep 1
  hid_task("\x05\000\000\000\000\000")
  sleep 0.01
  hid_task("")
  sleep 1
  hid_task("\x07\000\000\000\000\000")
  sleep 0.01
  hid_task("")
  sleep 1
end
