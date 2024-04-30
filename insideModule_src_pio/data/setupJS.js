
const submitButtonElement = document.getElementById('submit');
const addButtonElement = document.getElementById('add');
const modulesInputContainerElement = document.getElementById('modules-data');
let modulesCount = 0;
const dataURL = '/api/data';

const validator = (field) => field.value == '' || field.value == null || /^\s*$/.test(field.value);

function createModule(name = '', height = 80, power = 'Battery') {
    const moduleDataElement = document.createElement('div');
    moduleDataElement.classList.add('module-data');
    // Create h3 element
    const h3 = document.createElement('h3');
    h3.textContent = `New module`;

    //Create deleteButtonElement:
    const deleteButton = document.createElement('button');
    deleteButton.textContent = "Remove";
    deleteButton.classList.add('delete-btn');
    deleteButton.addEventListener('click', e => {
        deleteButton.parentElement.parentElement.removeChild(moduleDataElement);
        modulesCount--;
    });

    // Create label and input elements for module name
    const nameLabel = document.createElement('label');
    nameLabel.setAttribute('for', `module-name-${name}`);
    nameLabel.textContent = 'Name:';
    const nameInput = document.createElement('input');
    nameInput.setAttribute('type', 'text');
    nameInput.setAttribute('id', `module-name-${name}`);
    nameInput.setAttribute('name', 'moduleName');
    nameInput.setAttribute('value', name);

    //Handle validation:
    nameInput.addEventListener('input', e => {
        if (validator(nameInput)) {
            nameInput.classList.add('incorrect-data');
            submitButtonElement.setAttribute('disabled', 'disabled');
        } else {
            nameInput.classList.remove('incorrect-data');
            submitButtonElement.removeAttribute('disabled');
        }
    });

    // Create label and input elements for mounting height
    const heightLabel = document.createElement('label');
    heightLabel.setAttribute('for', `module-height-${name}`);
    heightLabel.textContent = 'Mounting height (cm):';
    const heightInput = document.createElement('input');
    heightInput.setAttribute('type', 'number');
    heightInput.setAttribute('id', `module-height-${name}`);
    heightInput.setAttribute('min', '50');
    heightInput.setAttribute('max', '110');
    heightInput.setAttribute('value', height);

    // Create label and select elements for power
    const powerLabel = document.createElement('label');
    powerLabel.setAttribute('for', `module-power-${name}`);
    powerLabel.textContent = 'Power:';
    const powerSelect = document.createElement('select');
    powerSelect.setAttribute('name', 'power');
    powerSelect.setAttribute('id', `module-power-${name}`);

    // Create options for the select element
    const batteryOption = document.createElement('option');
    batteryOption.setAttribute('value', 'battery');
    batteryOption.textContent = 'Battery';
    const pluggedInOption = document.createElement('option');
    pluggedInOption.setAttribute('value', 'pluggedIn');
    pluggedInOption.textContent = 'Plugged In';

    // Set the selected option
    if (power == 'battery') {
        batteryOption.setAttribute('selected', 'selected');
    } else {
        pluggedInOption.setAttribute('selected', 'selected');
    }

    // Append options to the select element
    powerSelect.appendChild(batteryOption);
    powerSelect.appendChild(pluggedInOption);

    // Append all elements to the container div
    moduleDataElement.appendChild(h3);
    moduleDataElement.appendChild(deleteButton);
    moduleDataElement.appendChild(nameLabel);
    moduleDataElement.appendChild(nameInput);
    moduleDataElement.appendChild(heightLabel);
    moduleDataElement.appendChild(heightInput);
    moduleDataElement.appendChild(powerLabel);
    moduleDataElement.appendChild(powerSelect);
    return moduleDataElement;
}

function onPageLoad() {
    // Your code here
    console.log("check check");
    //let json = '[{"name":"Out 1","height":55,"power":"battery"},{"name":"Out 2","height":43,"power":"pluggedin"}]';

    fetch(dataURL)
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.json();
        })
        .then(data => {
            console.log(data);
            modulesCount = data.length;
            modules = data;
            updateModuleCards(data);
        })
        .catch(error => {
            console.error('There was a problem with the fetch operation:', error);
        });


}

function updateModuleCards(modules) {
    //clear container:
    modulesInputContainerElement.textContent = "";

    for (let i = 1; i <= modules.length; i++) {
        const currentModule = modules[i - 1];
        const moduleDataElement = createModule(currentModule.name, currentModule.height, currentModule.power);
        console.log(moduleDataElement);
        modulesInputContainerElement.appendChild(moduleDataElement);
    }
}

addButtonElement.addEventListener('click', e => {
    if (modulesCount < 6) {
        const newModuleElement = createModule();
        modulesInputContainerElement.appendChild(newModuleElement);
        modulesCount++;
    }
});
submitButtonElement.addEventListener('click', async e => {
    //send post request with data:
    const modules = [];
    const modulesDivsElements = modulesInputContainerElement.querySelectorAll('div.module-data');
    console.log(modulesDivsElements);
    for (const moduleDivElement of modulesDivsElements) {
        //get the name, height and power:
        const name = moduleDivElement.querySelector('input[name=moduleName]').value;
        const height = moduleDivElement.querySelector('input[type=number]').value;
        const power = moduleDivElement.querySelector('select').value;
        console.log(name);
        console.log(height);
        console.log(power);
        modules.push({ name, height, power });
    }
    const json = JSON.stringify(modules);
    console.log(json);

    await fetch(dataURL, {
        method: "POST",
        headers: {
            'content-type': 'application/json'
        }
        ,
        body: json
    })
        .then(r => console.log(r))
        .catch(err => console.log(err));
   // location.reload();
});
