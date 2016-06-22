CodeMirror.defineMode('j', function(config) {

// must clear cache to get changed file downloaded
// jtheme.css selectors: global local control string comment

var c;
var words = {};
function define(style, string)
{
    var split = string.split(' ');
    for(var i = 0; i < split.length; i++) {
      words[split[i]] = style;
    }
};

  // Keywords
  define('control','for. goto. label. assert. break. continue. if. do. else. end. elseif. return. select. case. fcase. throw. try. while. whilst. catch. catchd. catcht.');

 function tokenBase(stream, state)
 {

 var sol = stream.sol();
 var ch = stream.next();

 if(ch === '\'')
 {
  if(stream.skipTo('\'')){stream.next();return'string';}
 }

 if(ch === 'N' && stream.eat('B') && stream.eat('.'))
 {
  c=stream.next();
  if(c!='.'&&c!=':')
  {
   stream.skipToEnd();return'comment';
  }
 }
 
 if(ch === '=' && stream.eat(':'))
 {
  c=stream.next();
  if(c==null)return'global';
  if(c!='.'&&c!=':')
  {
   stream.backUp(1);
   return'global';
  }
 }
 
 if(ch === '=' && stream.eat('.'))
 {
  c=stream.next();
  if(c==null)return'local';
  if(c!='.'&&c!=':')
  {
   stream.backUp(1);
   return'local';
  }
 }
 
 stream.eatWhile(/\w/);
 var cur = stream.current();

 var i=cur.indexOf("_");
 if(-1!=i)
  cur=cur.substr(0,i);
    
 if(stream.eat('.'))
 {
  if(stream.eol())
   cur=cur+'.'
  else
  {
   c=stream.next();
   if(c!='.'&&c!=':')
    cur=cur+'.';
   stream.backUp(1);
  }
 }

 return words.hasOwnProperty(cur) ? words[cur] : null;
}

  function tokenize(stream, state) {
    return (state.tokens[0] || tokenBase) (stream, state);
  };

  return {
    startState: function() {return {tokens:[]};},
    token: function(stream, state) {
      if (stream.eatSpace()) return null;
      return tokenize(stream, state);
    }
  };
});

