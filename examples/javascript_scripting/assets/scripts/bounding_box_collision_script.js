export function onCreate(entity) { }

export function onUpdate(entity) {
    const wSize = TooDeeEngine.renderTarget.size;
    const cTrans = entity.components.transform;
    const cCollider = entity.components.boundingBox;

    // Vertical
    if ((cTrans.pos.y - cCollider.halfSize.y < 0 && cTrans.velocity.y < 0) ||
        (cTrans.pos.y + cCollider.halfSize.y > wSize.y && cTrans.velocity.y > 0)) {
        cTrans.velocity.y *= -1;
    }
    // Horizontal
    if ((cTrans.pos.x - cCollider.halfSize.x < 0 && cTrans.velocity.x < 0) ||
        (cTrans.pos.x + cCollider.halfSize.x > wSize.x && cTrans.velocity.x > 0)) {
        cTrans.velocity.x *= -1;
    }
}

export function onDestroy(entity) { }
