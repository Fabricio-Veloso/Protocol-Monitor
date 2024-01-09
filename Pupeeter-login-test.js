/*Hi, I am Fabrício Veloso, the developer of this program. It is a simple webscraping server-side application.
Its objective is to be the main function that is called given a certain amount of time.
So the DB is kept updated.

Its gos to the web page, opens all the dropdowns menus so the protocol showe apears at the screen
and start to copy the data*/


//IMPORTS
  const config = require('./Config');
  const puppeteer = require('puppeteer');
  const delay = require('./Delay')
  const fs = require('fs');
//IMPORTS

//It just calls this main function
main();


//the Main function itself
async function main() {

  const { username, password, site } = config();
  // Lauch puppeteer (the headless option can be toggle to show the browser)
  const browser = await puppeteer.launch({
    headless: false,
   
  });

  //Creates a new page
  const page = await browser.newPage();

  //Goes to the destination site
  await page.goto(site);

  //waits for the login textbox
  const SeletorNomeUsuario = await page.waitForSelector('#username');

  //fills it with the user name provided
  await SeletorNomeUsuario.type(username);
  
  //waits for the password box
  const SeletorSenhaUsuario = await page.waitForSelector('#password');
  
  //Fills it with the password provided
  await SeletorSenhaUsuario.type(password);

  //Waits for the submit button
  const loginButton = await page.waitForSelector('#submit');
  
  //clicks it
  await loginButton.click();
  
  // waits for the navegation
  await page.waitForNavigation();

  //Now the browser is in the desired page.

  //To open the first menu we need to seach for it 
  const ProtocolLabel = await page.waitForXPath('//html/body/div[2]/div/div[4]/div/div[2]/div[5]')
  
  //and click it
  ProtocolLabel.click();

  //Loop sentinel variable
  let stop = 0;

  //creates an array to store the clicked buttons
  let clickedbuttons = [];

  //Output variable to store the data outuput
  let outuput = [];

  //Variable to store the quantity of tables
  let tableQuantity = 0;
  //While the sentinel variable value == 0(the loop should stop when all the drop down menus are open)
  while( stop == 0){

    await delay(1000);

    //recives all the buttons from the protocol label father element
    let protocolAjax = await page.waitForXPath('/html/body/div[2]/div/div[4]/div/div[2]/div[6]'); 
    
    //Creates a variable to store the buttons and fills it with al the buttons inside the father element
    let fatherElementButtons = await protocolAjax.$$('a');

    //creates a variable to store the filtered buttons
    let fatherElementFilteredButtons = [];

    //loop log variable
    let firstlooprols = 0;

    //filter the buttons to only the ones that we whant to press do save time
    for (const el of fatherElementButtons) {
      firstlooprols += 1;
      //Uses evaluate to get the Classname atribute from elements
      const className = await el.evaluate(element => element.className);

      //Uses evaluate to get the ID atribute from elements
      const id = await el.evaluate(element => element.id);
      
      // if the button is one that we intend to press it is pushed into the filtered buttons aray
      if(className =='botao_ini_ajax' || id == 'mostrarProtocolosAReceber' ||id == 'mostrarProtocoloSetorFilial0' || className == 'botao_ini_ajax mostrarRealFilial'){
        fatherElementFilteredButtons.push(el);
      }
    }

    //log mensage
    console.log('Filter loop rolled',firstlooprols);

    //resets the variable with 0
    firstlooprols = 0;
    
    //Log mensage to show how many elements are inside the fatherElemenButtons variable.
    console.log('the quantity of elements inside the father element recived is',fatherElementButtons.length,'\n', 'The quantity of elements filtered was',fatherElementFilteredButtons.length,'\n');

    //goes true all the elements for loggin purpose
    for (const el of fatherElementFilteredButtons) {

      //log mensage
      console.log('\n\n\nEl of fatherelementfilteredbuttons loop inicialized')
      
      //Uses evaluate the textcontent atribute from elements
      const textContent = await el.evaluate(element => element.textContent);

      //Uses evaluate the Classname atribute from elements
      const className = await el.evaluate(element => element.className);

      //Uses evaluate the ID atribute from elements
      const id = await el.evaluate(element => element.id);
      
      //My idea was to use each element's xpath to identify them since every id/class repeats true them
      //I thought that i could jut use evaluate to get it, bit seems that this attribute
      //is not embeded in the element's html and just make part of the DOMM tree.
      //I dont know how to do that, so i asked chat gpt how to get it. 
      //The code below is chat GPT made, it works just fine.

      //<GPT MADE>

      /*The evaluate method is used to execute a function in the context of the browser
       and it allows you to pass a JavaScript function that will be executed as if it were run in the browser console.
       The element parameter in this function represents the DOM element associated with el, which is each a element in your loop. */
      const xpath = await el.evaluate(element => {

      /*This is a recursive function called getXPath that calculates the XPath of a given DOM node.
      It takes a node as an argument and recursively traverses its parent nodes to build the XPath.*/
      function getXPath(node) {

      /*This line ensures that if the function is called with a falsy value or a null node
       it returns an empty string.*/  
      if (!node) return '';
      
      /*This line gets the node type of the current node. */
      const nodeType = node.nodeType;

      /*If the node is an element node (nodeType 1), 
      it calculates the XPath based on the position of the element among its siblings. */
      if (nodeType === 1) { // Element node

        /* This line calculates the index of the element among its parent's children. */
        const elementIndex = [...node.parentNode.children].indexOf(node) + 1;

        /* This line recursively calls getXPath on the parent node and appends the current element's tag name and index to the XPath.*/
        return getXPath(node.parentNode) + `/${node.tagName.toLowerCase()}[${elementIndex}]`;

        /* If the node is a text node (nodeType 3), it calculates the XPath based on the position of the text node among its siblings.*/
      } else if (nodeType === 3) { // Text node

        /*This line calculates the index of the text node among its parent's child nodes. */
        const textIndex = [...node.parentNode.childNodes].indexOf(node) + 1;

        /*This line recursively calls  */
        return getXPath(node.parentNode) + `/text()[${textIndex}]`;
        }

        /* If the node is neither an element nor a text node, an empty string is returned. */
        return '';
      }
        /*The main function returns the XPath by calling getXPath with the original element. */
        return getXPath(element);
      });

      // </GPT MADE>

      //Shows the content recived for loggin purpose
      console.log('Text Content:',textContent,
      '\n','Class Name:',className,'\n','element id',id,'\n','xpath',xpath,'\n');

      //coincidences variable to sinalize  if the button has already pressed
      let coincidences = 0;

      //Checks if the button have beeen already pressed by xpath comparison
      for(const xpathTest of clickedbuttons){

        //If the button has been already pressed 
        if(xpath === xpathTest){

          //Logs it
          console.log('There was acoicidence true the pressed buttons.\n');
          //console.log(xpath,'\n',xpathTest);

          //increments the coincidence variable
          coincidences = 1;

          break;
        }
      } 

      //if any no coincidences happend(the button is new and was not pressed)
      if(coincidences < 1 ){

        //logs that the buttons is new
        console.log('There was no coincidences the button is new\n');

        //Checks if the button is part of the one that we whant no click
        if(className =='botao_ini_ajax' ||
        id =='mostrarProtocolosAReceber'||
        id =='mostrarProtocoloSetorFilial0'||
        className == 'botao_ini_ajax mostrarRealFilial' ){
        
        //Performs a clic on it so the menu  opens
        await el.click();
        
        //Logs that the button was pressed
        console.log('Button pressed')

        //if the button is of a type that needs more time to be loaded whait more time
        if(className == 'botao_ini_ajax mostrarRealFilial' ){
        await delay(7000);

        //else whait the deful time
        }else{
          await delay(1000);
        }

        //stores the already pressed button so it wil not be pressed again in the future
        clickedbuttons.push(textContent,xpath);

        // logs the clicked buttons list
        console.log('Clicked buttons list',clickedbuttons,'\n'); 


        }else{
          //The button is not intended to be pressed
          console.log('the button is not intended to be pressed\n');
        } 
      }
    }
    
    //counts how much protocolShower Elements are at the page
    let tabelas = await page.$$('table');

    //logging tabelas quantity
    console.log('The tables quantity recived was',tabelas.length);

    // try catch to change the table ranges
    try{
    console.log('changing the tables ranges ');
    await page.waitForSelector('.dataTables_length select',{ timeout: 1000 });
    console.log(typeof '-1');
    await page.select('.dataTables_length select','-1');
    
    await delay(5000);
    }
    catch (error){
      console.log('table lenght element not found, continuing',error);
      continue;
    }

    // Loop sobre cada tabela
    for (const tabelaHandle of tabelas) {
      // Extrair cabeçalhos
      const headers = await tabelaHandle.$$eval('th', ths => ths.map(th => th.textContent.trim()));

      // Extrair dados das células
      const dados = await tabelaHandle.$$eval('tbody tr', linhas =>
        linhas.map(linha =>
          Array.from(linha.getElementsByTagName('td')).map(celula => celula.textContent.trim())
        )
      );

      // Agora você tem os cabeçalhos e os dados para cada tabela
      console.log('Headers:', headers);
      console.log('');
      console.log('Dados:', dados);

      //pushes it to the outuput variable to be send to the bd or txtdocument
      outuput.push('headers:',headers,'/n','data:',dados);

      // Fechar o punho da tabela
      await tabelaHandle.dispose();

    }

    //Logs the amount of protocolshower elements ant the screen
    console.log('The quantity of protocol showers are:',tabelas.length,'\n\n\n');

    //trys to evaluate if the quantity of tables changed to stop the loop
    try{ 
      //if the are tables at the screen
      if(tabelas.length > 0){
        //if the previus tables quantity was not the same 
        if(tableQuantity != tabelas.length) {

          //there was a new table discovered, the quantity is updated.
          tableQuantity = tabelas.length;
        }else{
          console.log('the is no new tables at the screen, endding the loop')
          stop = 1;
        }
      }
    }  
    catch (error) {
      console.log('The evaluate tables quantity was not sucessful');
      
    }
    //stores the pages tables quantity
    
    
    

    //sends the signal to stop the loop
    //stop = 1;

    
  }
  //Log mensage
  console.log('The loop ended');

  //Closes the browser
  await browser.close();
  console.log('\n\n')
}









