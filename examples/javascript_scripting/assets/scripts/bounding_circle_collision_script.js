export function onCreate(entity) { }

export function onUpdate(entity) {
    const wSize = TooDeeEngine.renderTarget.size;
    /*
    const cTrans = entity.components.transform;
    const cCollider = entity.components.boundingCircle;

    // Vertical
    if ((cTrans.pos.y - cCollider.radius < 0 && cTrans.velocity.y < 0) ||
        (cTrans.pos.y + cCollider.radius > wSize.y && cTrans.velocity.y > 0)) {
        cTrans.velocity.y *= -1;
    }
    // Horizontal
    if ((cTrans.pos.x - cCollider.radius < 0 && cTrans.velocity.x < 0) ||
        (cTrans.pos.x + cCollider.radius > wSize.x && cTrans.velocity.x > 0)) {
        cTrans.velocity.x *= -1;
    }
        */
}

export function onDestroy(entity) { }
