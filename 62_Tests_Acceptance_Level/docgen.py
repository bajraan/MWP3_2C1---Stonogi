import json
import datetime
import pandas as pd
import re

now = datetime.datetime.now()
timestamp = now.strftime("%Y-%m-%d_%H-%M-%S")
filename = f"AcceptanceLevel_TestRaport_{timestamp}.html"

print(f"Data reading form input.JSON file")
with open("input.json", "r") as f:
    data = json.load(f)
    expected_values = [data[k]['Expected'] for k in data.keys()]

print(f"Data reading form stdout.txt file")
df = pd.read_csv('stdout.txt', header=None)

print(f"Diagram reading form MainAlgorithm_BC.drawio.svg file")
with open("../70_Documentation/MainAlgorithm_BC.drawio.svg", "r") as file:
    # Read the file and skip the first two lines
    file_lines = file.readlines()[2:]
    # Join the remaining lines into a string
    modified_file = "".join(file_lines)
    orginal_file = "".join(file_lines)

print(f"Reading Logo.svg file")
with open("../70_Documentation/Logo.svg", "r") as file:
    # Read the file and skip the first two lines
    file_lines = file.readlines()[2:]
    # Join the remaining lines into a string
    Logo = "".join(file_lines)


for key, value in data.items():
    pattern = f'({key}.*?opacity=")0.4(")'
    modified_file = re.sub(pattern, rf'\g<1>0.0"', modified_file)


print(f"Checking results: ")
for i, (value, expected) in enumerate(zip(df[0], expected_values)):
    if value == expected:
        print(f"Test {i+1}: PASSED")
    else:
        print(f"Test {i+1}: FAILED")

print(f"Creating .HTML file")
now = datetime.datetime.now()
timestamp = now.strftime("%Y-%m-%d_%H-%M-%S")
date = now.strftime("%Y-%m-%d")
filename = f"AcceptanceLevel_TestRaport_{timestamp}.html"

html ="""
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>Test Raport Acceptance Level</title>

    <style>

    body {
        background-color: #f2f2f2;
      }

    #logo
    {
      width: 1700px;
      height: auto;
      background-color: #f2f2f2;
      margin-left: auto;
      margin-right: auto;
    }
    #Title
    {
      width: 1700px;
      height: auto;
      background-color: #f2f2f2;
      margin-left: auto;
      margin-right: auto;
      font-size: 100px;
      text-align: center;
      padding: 200px;
    }
    #tccontainer
    {
      width: 1700px;
      height:2600px;
      margin-left: auto;
      margin-right: auto;
    }
    #space_black
    {
      margin-left: auto;
      margin-right: auto;
      width: 1700px;
      height:20px;
      background-color: #f2f2f2;
      vertical-align: middle;
    }
	  #space_white
    {
      margin-left: auto;
      margin-right: auto;
      width: 1700px;
      height:20px;
      vertical-align: middle;
    }
    #summary
    {
      margin-left: auto;
      margin-right: auto;
      width: 1700px;
      height:auto;
      vertical-align: middle;
    }

    #container_left
    {
      float: left;
      width: 750px;
      height: 2350px;
      font-size: 1.2cm;
    }
    #container_right
    {
      float: left;
      width: 500px;
      height: 1500px;
      background-color: green;
    }
    #details
    {
      float: none
      height: 1200px;
      padding: 50px
    }

    #corddraw
    {
    display: flex;
    justify-content: center;
    align-items: center;
    }

    </style>
  </head>

  <body>
   
    <div id="logo">"""
html+=Logo      
html+="""
  </div>
    <div id="Title">
      Acceptance Level
      <br><br> Test Raport
      <br><br> MWP3_2C1 - Stonogi
      <br><br> {date}
    </div>
""".format(date=date)

idd = 0
for key, value in data.items():
    ST1_P1X = value['wektorA_P1X']
    ST1_P1Y = value['wektorA_P1Y']
    ST1_P2X = value['wektorA_P2X']
    ST1_P2Y = value['wektorA_P2Y']
    ST1_spe = value['wektorA_speed']
    ST2_P1X = value['wektorB_P1X']
    ST2_P1Y = value['wektorB_P1Y']
    ST2_P2X = value['wektorB_P2X']
    ST2_P2Y = value['wektorB_P2Y']
    ST2_spe = value['wektorB_speed']
    Exp = value['Expected']
    Obs = df.iloc[idd, 0]

    print(f"{key}:{ST1_P1X},{ST1_P1Y};{ST1_P2X},{ST1_P2Y};{ST1_spe}\
                  {ST2_P1X},{ST2_P1Y};{ST2_P2X},{ST2_P2Y};{ST2_spe};{Exp}")
    

    if df.iloc[idd, 0] == "???":
        Res = """
        <div style="font-weight:bold;background-color:black;color:red;padding:2%;">UNKNOWN</div>"""
    elif expected_values[idd] == df.iloc[idd, 0]:
        Res = """
        <div style="font-weight:bold;background-color:green;color:orange;padding:2%;">PASSED</div>"""
    else:
        Res = """
        <div style="font-weight:bold;background-color:red;color:black;padding:2%;">FAILED</div>"""

    idd+=1   

    testcase = """
    <div id="tccontainer">

      <div id="space_black">
      </div>
      <div id="space_white">
      </div>

      <div id="container_left">

        <div id="details">
                 TC Name: <b>{name}</b>
            <br>
            <br> Status: {test_result}
            <br> Input Data
            <br> Stonoga 1 P1 ({s1p1x},{s1p1y})
            <br> Stonoga 1 P2 ({s1p2x},{s1p2y})
            <br> Stonoga 1 Speed: {s1spe}
            <br> Stonoga 2 P1 ({s2p1x},{s2p1y})
            <br> Stonoga 2 P2 ({s2p2x},{s2p2y})
            <br> Stonoga 2 Speed: {s2spe}
            <br>
            <br> stdin:
            <br> 1
            <br> {s1p1x} {s1p1y} {s1p2x} {s1p2y} {s1spe}
            <br> {s2p1x} {s2p1y} {s2p2x} {s2p2y} {s2spe}
            <br>
            <br> expected stdout:
            <br> {expected_result}
            <br> observed stdout:
            <br> {observed_result}   
        </div>

        <div id="corddraw">
            <canvas id="Canvas{name}" width="700" height="700"></canvas>

            <script>
              var canvas = document.getElementById("Canvas{name}");
              var ctx = canvas.getContext("2d");
              var wersalikhight = 20;

              // Axis X
              ctx.beginPath();
              ctx.moveTo(0, canvas.height/2);
              ctx.lineTo(canvas.width, canvas.height/2);
              ctx.lineWidth = 4; 
              ctx.strokeStyle = "black";
              ctx.stroke();

              // Axis X descriptions
              for(var i = 50; i <= canvas.width; i += 50) 
              {{
                  ctx.fillText((i - canvas.width/2)/50, i, canvas.height/2 + 20);
              }}

              // Axis Y
              ctx.beginPath();
              ctx.moveTo(canvas.width/2, 0);
              ctx.lineTo(canvas.width/2, canvas.height);
              ctx.strokeStyle = "black";
              ctx.stroke();

              // Axis Y descriptions
              for(var i = 50; i <= canvas.height; i += 50) 
              {{
                  if(i != canvas.height/2) {{
                      ctx.fillText((canvas.height/2 - i)/50, canvas.width/2 + 10, i);
                  }}
              }}

              // Grid
              ctx.strokeStyle = "rgba(0, 0, 0, 0.1)";
              ctx.strokeStyle = "rgba(0, 0, 0, 0.1)";
              for(var i = 50; i <= canvas.width; i += 50)
              {{
              ctx.beginPath();
              ctx.moveTo(i, 0);
              ctx.lineTo(i, canvas.height);
              ctx.stroke();
              }}
              for(var i = 50; i <= canvas.height; i += 50)
              {{
              ctx.beginPath();
              ctx.moveTo(0, i);
              ctx.lineTo(canvas.width, i);
              ctx.stroke();
              }}

              // Stonoga 1
              var startX =  ({s1p1x})*50+350;
              var startY = -({s1p1y})*50+350;
              var endX =    ({s1p2x})*50+350;;
              var endY =   -({s1p2y})*50+350;;

              // Vector Drawing
              ctx.beginPath();
              ctx.moveTo(startX, startY);
              ctx.lineTo(endX, endY);
              ctx.lineWidth = 5;
              ctx.strokeStyle = "red";
              ctx.stroke();

              // Triangle End
              var angle = Math.atan2(endY - startY, endX - startX);
              var arrowSize = 20;
              ctx.beginPath();
              ctx.moveTo(startX, startY);
              ctx.lineTo(startX + arrowSize * Math.cos(angle - Math.PI / 6), startY + arrowSize * Math.sin(angle - Math.PI / 6));
              ctx.lineTo(startX + arrowSize * Math.cos(angle + Math.PI / 6), startY + arrowSize * Math.sin(angle + Math.PI / 6));
              ctx.closePath();
              ctx.fillStyle = "red";
              ctx.fill();

              // Speed Value
              var centerX = (startX + endX) / 2;
              var centerY = (startY + endY) / 2;
              var label = "{s1spe}";
              var legend = "ST1_V = {s1spe}";
              ctx.font = "bold 16px Arial";
              ctx.textAlign = "center";
              ctx.fillText(label, centerX, centerY);
              ctx.font = "bold 32px Arial";
              ctx.textAlign = "left";
              ctx.fillText(legend, 50, 50);

              //Stonoga 2
              var startX =  ({s2p1x})*50+350;
              var startY = -({s2p1y})*50+350;
              var endX =    ({s2p2x})*50+350;;
              var endY =   -({s2p2y})*50+350;;

              //Vector Drawing
              ctx.beginPath();
              ctx.moveTo(startX, startY);
              ctx.lineTo(endX, endY);
              ctx.lineWidth = 5;
              ctx.strokeStyle = "blue";
              ctx.stroke();

              // Triangle End
              var angle = Math.atan2(endY - startY, endX - startX);
              var arrowSize = 20;
              ctx.beginPath();
              ctx.moveTo(endX, endY);
              ctx.lineTo(endX - arrowSize * Math.cos(angle - Math.PI / 6), endY - arrowSize * Math.sin(angle - Math.PI / 6));
              ctx.lineTo(endX - arrowSize * Math.cos(angle + Math.PI / 6), endY - arrowSize * Math.sin(angle + Math.PI / 6));
              ctx.closePath();
              ctx.fillStyle = "blue";
              ctx.fill();

              // Speed Value
              var centerX = (startX + endX) / 2;
              var centerY = (startY + endY) / 2;
              var label = "{s2spe}";
              var legend = "ST2_V = {s2spe}";
              ctx.font = "bold 16px Arial";
              ctx.textAlign = "center";
              ctx.fillText(label, centerX, centerY);
              ctx.font = "bold 32px Arial";
              ctx.textAlign = "left";
              ctx.fillText(legend, 50, 100);
            </script>

        </div>
      </div>
      <div id="container_right">
     """.format(name=key,\
                s1p1x=ST1_P1X,\
                s1p1y=ST1_P1Y,\
                s1p2x=ST1_P2X,\
                s1p2y=ST1_P2Y,\
                s2p1x=ST2_P1X,\
                s2p1y=ST2_P1Y,\
                s2p2x=ST2_P2X,\
                s2p2y=ST2_P2Y,\
                s1spe=ST1_spe,\
                s2spe=ST2_spe,\
                expected_result=Exp,\
                observed_result=Obs,\
                test_result=Res)
    
    html+=testcase

    pattern = f'({key}.*?opacity=")0.0(")'
    modified_file = re.sub(pattern, rf'\g<1>1.0"', modified_file)

    html+=modified_file

    pattern = f'({key}.*?opacity=")1.0(")'
    modified_file = re.sub(pattern, rf'\g<1>0.0"', modified_file)

    html+="""
        </div>
      </div> """
    
html += "    <div id=summary>"
html += orginal_file
html += "</div>"


html+="""
  </body>
</html>"""

with open(filename, "w", encoding="utf-8") as f:
    f.write(html)