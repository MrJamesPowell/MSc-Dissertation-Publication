import processing.serial.*; 

Serial myPort;
PrintWriter output;
int startTime;

void setup() {
  // List all available ports
  printArray(Serial.list());

  // Pick the right port (change [0] if needed)
  myPort = new Serial(this, Serial.list()[7], 9600);

  // Create timestamped filename
  String filename = year() + "-" + nf(month(),2) + "-" + nf(day(),2) +
                    "_" + nf(hour(),2) + "-" + nf(minute(),2) + "-" + nf(second(),2) +
                    ".csv";
  output = createWriter(filename);

  // Record the start time
  startTime = millis();

  println("Saving to file: " + filename);
}

void draw() {
  if (myPort.available() > 0) {
    String data = myPort.readStringUntil('\n');
    if (data != null) {
      data = trim(data);

      // Time passed since start (in seconds, with decimals)
      float elapsed = (millis() - startTime) / 1000.0;

      // Save with elapsed time
      String line = nf(elapsed, 0, 3) + " , " + data; // 3 decimals
      println(line);
      output.println(line);
      output.flush();
    }
  }
}

void keyPressed() {
  output.close();
  exit();
}
