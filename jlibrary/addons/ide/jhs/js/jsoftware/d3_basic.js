// globals
var az;    // height of header+title+legend+footer
var m;     // margin
var mh;    // height margin
var ww;    // box container width
var wh;    // box container height
var ss;    // slop to avoid scroll bars
var dmin=0,dmax=0;

function color(n){return d3.scaleOrdinal(d3.schemeCategory10).range()[n];}
function getlabel(i){return (i<label.length)?label[i]:i;}

function seterr(g,t){jbyid(g+"_error").innerHTML= "D3 error - "+g+" - "+t;}

function plot(g,tabdata)
{
 common(g,tabdata);
 switch(type)
 {
 case "line":  plotline(g);break;
 case "pie":   plotpie(g);break;
 case "bar":   plotbar(g);break;
 case "error": break;
 default:      seterr(g,"plot type not supported"); break;
 }
}

function common(g,tabdata)
{
// set default plot parameters before eval of explicit ones
type= "line";
header="";
title= "";
legend= [];
label= [];
footer="";
linewidth= 1;
barwidth= 40;
minh= 200;
maxh= 400;
minw= 200;
maxw= 400;
data=[[10,100,30]];  // data matrix - list of lists

// vars not set by eval
datan='';
az=0;   // height of header+footer
m= 50;  // margins
mh= 10; // height margin
ss= 0;  // slop to avoid scrollbars
ww=0;   // box container width
wh=0;   // box container height
w=0;    // graph width
h=0;    // graph height
dmin=0;
dmax=0;

var s=tabdata.split('\n');
for(var i=0;i<s.length;++i)
{
 try{eval(s[i]);}catch(e)
 {
  seterr(g,"eval failed:<br>"+s[i]);
 }
}

var t= ""; var s= "";
var c=legend.length;
for(var i=0;i<c;++i)
{
 t+= s+"<span style=\"color:"+color(i)+";\">"+legend[i]+"</span>";
 s= "&nbsp;&bull;&nbsp;";
}

jbyid(g+"_header").innerHTML=header;
jbyid(g+"_title").innerHTML=title;
jbyid(g+"_legend").innerHTML= t;
jbyid(g+"_footer").innerHTML=footer;
 
 for(var i=0;i<data.length;++i)
  for(var j=0;j<data[0].length;++j)
  {
   dmin= (dmin<data[i][j])?dmin:data[i][j];
   dmax= (dmax>data[i][j])?dmax:data[i][j];
  }

 az= $("#"+g+"_header").height()+$("#"+g+"_title").height();
 az= az+$("#"+g+"_legend").height() + $("#"+g+"_footer").height(); 
 
// ww,wh are for box container
ww= jbyid(g+"_box").style.width;
ww= ww.substr(0, ww.length-2);

wh= jbyid(g+"_box").style.height;
wh= wh.substr(0, wh.length-2);
 
w= ww-(m+m+ss); // width needs more slop than height
h= wh-(az+mh+mh+ss);

h= (h<minh)?minh:h;
h= (h>maxh)?maxh:h;

w= (w<minw)?minw:w;
w= (w>maxw)?maxw:w;

$("#"+g).html(""); // remove this to see multiple graphs
}
 
function plotline(g)
{
 var x= d3.scaleLinear().domain([0,-1+data[0].length]).range([0,w]);
 var y= d3.scaleLinear().domain([dmin,dmax]).range([h,0]);

 var line = d3.line()
   .x(function(d,i){return x(i);})
   .y(function(d){return y(d);})

   var graph = d3.select("#"+g).append("svg:svg")
 
    .attr("width",w+m+m)
    .attr("height",h+mh+mh)
    .append("svg:g")
    .attr("transform","translate("+m+","+mh+")");

 var xAxis = d3.axisBottom().scale(x).tickSize(-h);
   graph.append("svg:g").attr("class", "x axis").attr("transform", "translate(0,"+h+")").call(xAxis);

 var yAxisLeft = d3.axisLeft().scale(y).ticks(4); // .orient("left"); // left y axis
   graph.append("svg:g").attr("class", "y axis").attr("transform", "translate(-25,0)").call(yAxisLeft);
  
 for(var i=0;i<data.length;++i)
 {
  datan= data[i];
  var q= graph.append("svg:path");
  q.style("stroke",color(i));
  q.style("stroke-width",linewidth);
  q.attr("d",line(datan));
 }
}

function plotpie(g)
{
 w= w+2*m-mh; // adjust for kludge for line plot width
 
 var q= (w>h)?h:w;
 q= (q<minh)?minh:q;
 q= (q>maxh)?maxh:q;
 q= q-(2*m);
 q= q-q%2;
 w = q;
 h = q;
 
 var r= q/2;
 datan=data[0];
    
 var vis = d3.select("#"+g)
  .append("svg:svg")
  .data([datan])
  .attr("width",w+m+m)
  .attr("height",h+m+m)
  .append("svg:g")
  .attr("transform", "translate("+(r+m)+","+(r+m)+")")
 
 var arc = d3.arc().outerRadius(r).innerRadius(0);
 
 //var pie = d3.layout.pie()
 var pie = d3.pie()
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
}

function plotbar(g)
{
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
// var h= wh-(az+m+m+ss);
h= (h<minh)?minh:h;
h= (h>maxh)?maxh:h;

var p= [m,m,m,m], // 20 50 30 20
 //d3.scaleBand().rangeRound([range]);
 //   x= d3.scaleOrdinal().rangeRoundBands([0, w - p[1] - p[3]]),
    x= d3.scaleBand().rangeRound([0, w - p[1] - p[3]]),
    y= d3.scaleLinear().range([0, h - p[0] - p[2]])
    z= color;

var svg = d3.select("#"+g)
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
//.attr("width", x.rangeBand());
.attr("width", x.bandwidth());
//.attr('width', xScale.bandwidth())
      
      
var dolabel = svg.selectAll("text")
 .data(x.domain())
 .enter().append("svg:text")
 .attr("x", function(d) { return x(d) + x.bandwidth() / 2; })
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
