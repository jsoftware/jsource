coclass'jd3'
coinsert'jhs'

HBS=: 0 : 0
'<link rel="stylesheet" href="~addons/ide/jhs/js/jquery/smoothness/jquery-ui.custom.css" />'
'<script src="~addons/ide/jhs/js/jquery/jquery-2.0.3.min.js"></script>'
'<script src="~addons/ide/jhs/js/d3/d3.v3.min.js"></script>'
'<div id="d3error"></div>'
'<div id="ahtml"></div>'
'<div id="graph"></div>'
'<div id="legend"></div>'
'<div id="zhtml"></div>'
'<div id="dialog" title="D3 Error"></div>'
)

jev_get=: 3 : 0
'jd3'jhr''
)

NB. wid_body_load_data_jtable_ values are not cleaned up
ev_body_load=: 3 : 0
try.
 a=. '_ev_body_load_data_jd3_',~getv'jwid' 
 if. 0~:nc<a do. (a)=: '[1,2,3]' end.
 d=. a~
catch.
 jhrajax'bad argument'
 return.
end. 
jhrajax JASEP,d
)

CSS=: 0 : 0
path{stroke: steelblue;stroke-width: 1;fill: none;}
.axis{shape-rendering:crispEdges;}
.x.axis line{stroke:lightgrey;}
.x.axis .minor{stroke-opacity:.5;}
.x.axis path{display:none;}
.y.axis line, .y.axis path{fill:none;stroke:#000;}
.slice text{font-size:16pt;font-family:Arial;}
#d3error{font-size:24pt;margin-left:20px;margin-top:20px;}
span{font-size:inherit;}
)

JS=: 0 : 0 NB. javascript
// vars set from J with ajax eval
title= "title";
titlesize= "24pt"
type= "line";
minh= 0;
maxh= 2000;
linewidth= 1;
barwidth= 40;
legend= [];
label= [];
data=[[]];  // data matrix - list of lists


//window.onresize= plot;
function color(n){return d3.scale.category20().range()[n];}
function getlabel(i){return (i<label.length)?label[i]:i;}

// must use JHS framework load handler instead of jquery - $(document).ready(function() 
function ev_body_load()
{
document.title= window.name;
jdoajax([]);
}

function seterr(t)
{
  $("#d3error").html("J D3 error - "+t);
  type="error"; // prevent resize from drawing
}

function ajax(ts)
{
 if(0!=(ts[0].length)){seterr("ajax failed: "+ts[0]); return;}
 var s= ts[1].split('\n');
 for(var i=0;i<s.length;++i)
 {
  try{eval(s[i]);}catch(e)
  {
   seterr("eval failed:<br>"+s[i]);
  }
 }
 common1(); 
 plot();
 window.onresize= plot; // now we can resize
}

// vars not set from J with ajax eval
var datan; // a row from data
var az;    // height of header+footer
var m;     // margin
var ww;    // window width
var wh;    // window height less header+footer\
var ss;    // slop to avoid scroll bars
var dmin=0,dmax=0;

function plot()
{
 $("#graph").html(""); // remove this to see multiple graphs
 switch(type)
 {
 case "line":  plotline();break;
 case "pie":   plotpie();break;
 case "bar":   plotbar();break;
 case "error": break;
 default:      seterr("plot type not supported"); break;
 }
}

function common1()
{
 var t= "";
 var s= "";
 
 var c= ("pie"==type)?data[0].length:data.length;
 for(var i=0;i<c;++i)
 {
  t+= s+"<span style=\"color:"+color(i)+";\">"+legend[i]+"</span>";
  s= "&nbsp;&bull;&nbsp;";
 }
 $("#legend").html(t);
 
 for(var i=0;i<data.length;++i)
  for(var j=0;j<data[0].length;++j)
  {
   dmin= (dmin<data[i][j])?dmin:data[i][j];
   dmax= (dmax>data[i][j])?dmax:data[i][j];
  }
}

function common()
{
 $("#graph").html(""); // remove this to see multiple graphs
 az= $("#ahtml").height()+$("#zhtml").height()+$("#legend").height();
 m= 50; // margins
 ww= window.innerWidth;
 wh= window.innerHeight;
 ss= 40;
}
 
function plotline()
{
 common();
 var w= ww-(m+m+ss);
 var h= wh-(az+m+m+ss);
 h= (h<minh)?minh:h;
 h= (h>maxh)?maxh:h;
 
 var x= d3.scale.linear().domain([0,-1+data[0].length]).range([0,w]);
 var y= d3.scale.linear().domain([dmin,dmax]).range([h,0]);

 var line = d3.svg.line()
   .x(function(d,i){return x(i);})
   .y(function(d){return y(d);})

 var graph = d3.select("#graph").append("svg:svg")
    .attr("width",w+m+m)
    .attr("height",h+m+m)
    .append("svg:g")
    .attr("transform","translate("+m+","+m+")");

 var xAxis = d3.svg.axis().scale(x).tickSize(-h).tickSubdivide(true);
   graph.append("svg:g").attr("class", "x axis").attr("transform", "translate(0,"+h+")").call(xAxis);

 var yAxisLeft = d3.svg.axis().scale(y).ticks(4).orient("left"); // left y axis
   graph.append("svg:g").attr("class", "y axis").attr("transform", "translate(-25,0)").call(yAxisLeft);
  
 for(var i=0;i<data.length;++i)
 {
  datan= data[i];
  var q= graph.append("svg:path");
  q.style("stroke",color(i));
  q.style("stroke-width",linewidth);
  q.attr("d",line(datan));
 }

 set_title(graph,w/2,0-(m/2),"middle",titlesize,title);
}

function set_title(g,x,y,anchor,size,title)
{
 g.append("text").attr("x",x).attr("y",y).attr("text-anchor",anchor).style("font-size",size).text(title);
}

function plotpie()
{
 common();
 var w= ww;
 var h= wh-az;
 var q= (w>h)?h:w;
 q= (q<minh)?minh:q;
 q= (q>maxh)?maxh:q;
 q= q-(2*m);
 q= q-q%2;
 w = q;
 h = q;
 var r= q/2;
 datan=data[0];
    
 var vis = d3.select("#graph")
  .append("svg:svg")
  .data([datan])
  .attr("width",w+m+m)
  .attr("height",h+m+m)
  .append("svg:g")
  .attr("transform", "translate("+(r+m)+","+(r+m)+")")
 
 var arc = d3.svg.arc().outerRadius(r);
 
 var pie = d3.layout.pie()
  .value(function(d){return d;});
 
 var arcs = vis.selectAll("g.slice")
  .data(pie)
  .enter()
  .append("svg:g")
  .attr("class", "slice");
 
 arcs.append("svg:path")
  .style("fill", function(d, i){return color(i);})
  .attr("d", arc);
 
 arcs.append("svg:text")
  .attr("transform", function(d){
     d.innerRadius = 0;
     d.outerRadius = r;
     return "translate(" + arc.centroid(d) + ")";
    })
  .attr("text-anchor", "middle")
  .text(function(d, i) { return getlabel(i); }); 
  
 set_title(vis,0,-(r+10),"middle",titlesize,title);
}


function plotbar()
{
common();

// d3 examples often use csv file and the code depends on d3.csv data format
// rather than unravel the code - we make the ajax data conform to that format
var rows= data.length;
var cols= data[0].length;
var dat=new Array(rows);
var s=new Array(cols);
for(var i=0;i<rows;++i)
{
 dat[i] = new Array(cols);
 s[i]= new Array(cols);
} 
for(var i=0;i<rows;++i)
 for(var j=0;j<cols;++j)
 {
  if(i==0)
   s[i][j]= 0;
  else
   s[i][j]= s[i-1][j] + data[i-1][j]; 
  dat[i][j]= {x: getlabel(j) , y: data[i][j] , y0: s[i][j]}; 
}

w= barwidth*cols;
var h= wh-(az+m+m+ss);
h= (h<minh)?minh:h;
h= (h>maxh)?maxh:h;

var p= [m,m,m,m], // 20 50 30 20
    x= d3.scale.ordinal().rangeRoundBands([0, w - p[1] - p[3]]),
    y= d3.scale.linear().range([0, h - p[0] - p[2]])
    z= color;

var svg = d3.select("#graph")
 .append("svg:svg")
 .attr("width", w)
 .attr("height", h)
 .append("svg:g")
 .attr("transform", "translate(" + p[3] + "," + (h - p[2]) + ")");

x.domain(dat[0].map(function(d) { return d.x; }));
y.domain([0, d3.max(dat[dat.length - 1], function(d) { return d.y0 + d.y; })]);

var cause = svg.selectAll("g.cause")
 .data(dat)
 .enter().append("svg:g")
 .attr("class", "cause")
 .style("fill", function(d, i) { return z(i); })
 .style("stroke", function(d, i) { return d3.rgb(z(i)).darker(); });

var rect = cause.selectAll("rect")
 .data(Object)
 .enter().append("svg:rect")
 .attr("x", function(d) { return x(d.x); })
 .attr("y", function(d) { return -y(d.y0) - y(d.y); })
 .attr("height", function(d) { return y(d.y); })
.attr("width", x.rangeBand());
      
      
var dolabel = svg.selectAll("text")
 .data(x.domain())
 .enter().append("svg:text")
 .attr("x", function(d) { return x(d) + x.rangeBand() / 2; })
 .attr("y", 6)
 .attr("text-anchor", "middle")
 .attr("dy", ".71em")
 .text(function(d) { return d; });

var rule = svg.selectAll("g.rule")
 .data(y.ticks(5))
 .enter().append("svg:g")
 .attr("class", "rule")
 .attr("transform", function(d) { return "translate(0," + -y(d) + ")"; });

rule.append("svg:line")
 .attr("x2", w - p[1] - p[3])
 .style("stroke", function(d) { return d ? "#fff" : "#000"; })
 .style("stroke-opacity", function(d) { return d ? .7 : null; });

rule.append("svg:text")
 .attr("x", w - p[1] - p[3] + 6)
 .attr("dy", ".35em")
 .text(d3.format(",d"));
}

)