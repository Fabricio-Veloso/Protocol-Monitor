async function delay(ms) {
    console.log('waiting',ms/1000,'s');
    return new Promise(resolve => setTimeout(resolve, ms));
    
}
module.exports = delay;