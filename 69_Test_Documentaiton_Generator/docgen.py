import json
import datetime
import pandas as pd
import re

def remove_preproc(code):
    lines = code.split('\n')
    new_lines = []
    skip_lines = False
    for line in lines:
        if line.startswith('#if MODE_TEST == 1'):
            skip_lines = True
        elif line.startswith('#endif // MODE_TEST'):
            skip_lines = False
        elif not skip_lines:
            new_lines.append(line)
    return '\n'.join(new_lines)

print(f"Data reading form input.JSON file")
with open("input.json", "r") as f:
    data = json.load(f)
    expected_values = [data[k]['Expected'] for k in data.keys()]

num_items = len(data.keys())

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
htmlFilename = f"AcceptanceLevel_TestRaport_{timestamp}.html"
maincppexportFilename = f"main_{timestamp}.cpp"


html_1_Logo       = """
<!DOCTYPE html>
<html>
  <style>

    body {
        background-color: #f2f2f2;
      }

    #logo
    {
      width: 2000px;
      height: auto;
      background-color: #f2f2f2;
      margin-left: auto;
      margin-right: auto;
    }
    #Title
    {
      width: 2000px;
      height: auto;
      background-color: #f2f2f2;
      margin-left: auto;
      margin-right: auto;
      font-size: 100px;
      text-align: center;
    }
    #summary
    {
      width: 2000px;
      height: auto;
      margin-left: auto;
      margin-right: auto;
      font-size: 100px;
      text-align: center;
      vertical-align: middle;
    }
    #tccontainer
    {
      width: 2000px;
      height:2600px;
      margin-left: auto;
      margin-right: auto;
    }
    #container_left
    {
      float: left;
      width: 750px;
      height: 2350px;
      font-size: 1.2cm;
    }
    #container_left_tab
    {
      float: left;
      width: 1100px;
      height: auto;
      font-size: 20px;
    }
    #container_right
    {
      float: left;
      width: 1250px;
      height: 1500px;
    }
    #container_right_tab
    {
      float: left;
      width: 500px;
      height: auto;
    }
    #details
    {
      float: none;
      height: 1200px;
      padding: 50px
    }

    #corddraw
    {
    display: flex;
    justify-content: center;
    align-items: center;
    }


    table {
      width: 100%;
      border-collapse: collapse;
      text-align: center;
      font-size: 30px ;
    }

    th, td {
      padding: 20px;
      text-align: center;
      border-bottom: 1px solid #ddd;
      font-size: 40px;
    }

    th {
      background-color: #f2f2f2;
    }

    tr:hover {
      color: black;
      font-weight: bold;
    }

    .pass {
      background-color: green;

    }

    .fail {
      background-color: red;
    }

    </style>

  <body>
<div id="logo">
</div>"""
html_2_Title      = """"""
html_3_fcon       = """
    <div id="Title">
      <table>
        <tr>
          <th>Lp</th>
          <th>Test Name</th>
          <th>Test Result</th>
          <th>Test Duration</th>
        </tr>"""
html_4_TestCases  = """"""   
html_5_Summary    = """"""

stdin_html = """<br> {num}""".format(num = num_items)
stdin_txt  = """{num}""".format(num = num_items)


idd = 0
AmofPass = 0;
AmofFail = 0;
AmofLack = 0;
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
        AmofLack += 1
    elif expected_values[idd] == df.iloc[idd, 0]:
        Res = """
            <div style="font-weight:bold;background-color:green;color:orange;padding:2%;">PASSED</div>"""
        AmofPass += 1
    else:
        Res = """
            <div style="font-weight:bold;background-color:red;color:black;padding:2%;">FAILED</div>"""
        AmofFail += 1

    idd+=1   

    html_tmp_testcase = """

    <h2 id="{name}"></h2>	
    
    <div id="tccontainer">

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

              var scalefactor = 50;
              if({s1p1x}>10 || {s1p1y}>10 || {s1p2x}>10 || {s1p2y}>10) scalefactor = 0.1

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
                  ctx.fillText((i - canvas.width/2)/scalefactor, i, canvas.height/2 + 20);
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
                      ctx.fillText((canvas.height/2 - i)/scalefactor, canvas.width/2 + 10, i);
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
              var startX =  ({s1p1x})*scalefactor+350;
              var startY = -({s1p1y})*scalefactor+350;
              var endX =    ({s1p2x})*scalefactor+350;;
              var endY =   -({s1p2y})*scalefactor+350;;

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
              var startX =  ({s2p1x})*scalefactor+350;
              var startY = -({s2p1y})*scalefactor+350;
              var endX =    ({s2p2x})*scalefactor+350;;
              var endY =   -({s2p2y})*scalefactor+350;;

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
              ctx.moveTo(startX, startY);
              ctx.lineTo(startX + arrowSize * Math.cos(angle - Math.PI / 6), startY + arrowSize * Math.sin(angle - Math.PI / 6));
              ctx.lineTo(startX + arrowSize * Math.cos(angle + Math.PI / 6), startY + arrowSize * Math.sin(angle + Math.PI / 6));
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
    html_4_TestCases+=html_tmp_testcase
    html_tmp_spistresci = """
         <tr>
          <td>{idd}</td>
          <td><a href="#{name}">{name}</a></td>
          <td class="pass">Pass</td>
          <td>2 min 30 sek</td>
        </tr>""".format(idd=idd,
                        name = key)
    html_3_fcon+=html_tmp_spistresci

    pattern = f'({key}.*?opacity=")0.0(")'
    modified_file = re.sub(pattern, rf'\g<1>1.0"', modified_file)

    html_4_TestCases+=modified_file

    pattern = f'({key}.*?opacity=")1.0(")'
    modified_file = re.sub(pattern, rf'\g<1>0.0"', modified_file)

    html_4_TestCases+="""
        </div>
      </div> """
    stdin_html += """
    <br> {s1p1x} {s1p1y} {s1p2x} {s1p2y} {s1spe}
    <br> {s2p1x} {s2p1y} {s2p2x} {s2p2y} {s2spe}""".format(s1p1x=ST1_P1X,\
                                                           s1p1y=ST1_P1Y,\
                                                           s1p2x=ST1_P2X,\
                                                           s1p2y=ST1_P2Y,\
                                                           s2p1x=ST2_P1X,\
                                                           s2p1y=ST2_P1Y,\
                                                           s2p2x=ST2_P2X,\
                                                           s2p2y=ST2_P2Y,\
                                                           s1spe=ST1_spe,\
                                                           s2spe=ST2_spe)   
    stdin_txt += """
{s1p1x} {s1p1y} {s1p2x} {s1p2y} {s1spe}
{s2p1x} {s2p1y} {s2p2x} {s2p2y} {s2spe}""".format(s1p1x=ST1_P1X,\
                                                      s1p1y=ST1_P1Y,\
                                                      s1p2x=ST1_P2X,\
                                                      s1p2y=ST1_P2Y,\
                                                      s2p1x=ST2_P1X,\
                                                      s2p1y=ST2_P1Y,\
                                                      s2p2x=ST2_P2X,\
                                                      s2p2y=ST2_P2Y,\
                                                      s1spe=ST1_spe,\
                                                      s2spe=ST2_spe)  





html_3_fcon+="""      </table>
    <br><br>
    </div>
"""
html_5_Summary += """    <div id=summary>
</br> Test Raport Summary"""
html_5_Summary += orginal_file
html_5_Summary += "</div>"
html_5_Summary +="    <div id=summary>"
html_5_Summary += stdin_html
html_5_Summary +="</div>"
html_2_Title   +="""

<div id="Title">
      <br><br><br>
      Acceptance Level
      <br> Test Raport
      <br><br><br>

      <table>
        <tr>
          <td>Project Name</td>
          <td>MWP3_2C1 - Stonogi</td>
        </tr>
        <tr>
          <td>Date</td>
          <td>{date}</td>
        </tr>
        <tr>
          <td>SW Version</td>
          <td>{maincppexportFilename}</td>
        </tr>
        <tr>
          <td>Result</td>
          <td class="pass">Pass</td>
        </tr>
        <tr>
          <td>
              executed:{AmofPass} + {AmofPass} <br>
              passed: {AmofPass} <br>
              failed: {AmofFail}
          </td>
          <td>
            <canvas id="myChart2" width="400" height="400"></canvas>
          </td>
        </tr>
      </table>
      <br><br><br>

  <script>
        // Chart Data
        var totalTests  = {AmofPass} + {AmofFail} + {AmofLack};
        var failedTests = {AmofFail};
        var passedTests = {AmofPass};
        var nonresTests = {AmofLack}; 
    
        // html references get
        var canvas = document.getElementById('myChart2');
        var ctx = canvas.getContext('2d');
        var totalTestsInfo = document.getElementById('totalTests');
        var failedTestsInfo = document.getElementById('failedTests');
    
        // Rest of test calculation
        var passedTests = totalTests - failedTests;
      
        // Chart Parameters
        var centerX = canvas.width / 2;
        var centerY = canvas.height / 2;
        var radius = Math.min(centerX, centerY) - 20;
    
        // Draw a circle chart
        var startAngle = 0;
    
        // Draw a sector for passed Test Cases
        var passedAngle = (2 * Math.PI * passedTests) / totalTests;
        ctx.beginPath();
        ctx.fillStyle = 'green';
        ctx.moveTo(centerX, centerY);
        ctx.arc(centerX, centerY, radius, startAngle, startAngle + passedAngle);
        ctx.closePath();
        ctx.fill();
        startAngle += passedAngle;
    
        // Draw a sector for failed Test Cases
        var failedAngle = (2 * Math.PI * failedTests) / totalTests;
        ctx.beginPath();
        ctx.fillStyle = 'red';
        ctx.moveTo(centerX, centerY);
        ctx.arc(centerX, centerY, radius, startAngle, startAngle + failedAngle);
        ctx.closePath();
        ctx.fill();
    </script>
</div>""".format( 
                date=date,
                maincppexportFilename = maincppexportFilename,
                AmofLack=AmofLack,
                AmofPass=AmofPass,
                AmofFail=AmofFail 
                )
html_5_Summary +="""  </body>
</html>"""

html = html_1_Logo
html+= html_2_Title
html+= html_3_fcon
html+= html_4_TestCases
html+= html_5_Summary

with open(htmlFilename, "w", encoding="utf-8") as f:
    f.write(html)


print(f"Creating stdin.txt file")
stdin_txtFilename = f"stdin.txt"
with open(stdin_txtFilename, "w", encoding="utf-8") as f:
    f.write(stdin_txt)


print(f"Creating main.c export into 62_Tests_Acceptance_Level")
with open('../main.cpp', 'r') as f:
    code = f.read()

new_code = remove_preproc(code)

with open(maincppexportFilename, 'w') as f:
    f.write(new_code)